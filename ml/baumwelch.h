/*
 * $Id: baumwelch.h 1306 2014-09-04 07:13:16Z justin $
 * Copyright (C) 2009 Lucid Fusion Labs

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LFL_ML_BAUMWELCH_H__
#define LFL_ML_BAUMWELCH_H__
namespace LFL {

struct BaumWelch : public HMM::BaumWelchAccum {
  struct Mode { enum { Means=1, Cov=2 }; };

  struct Accum {
    GMM mixture;
    double total, prior;

    typedef map<int, double> XiMap;
    XiMap transit;
    double transtotal;
    int count;

    Accum(int K, int D) : mixture(K, D) {}
  };

  AcousticModel::Compiled *model;

  int K, D;
  double totalprob, accumprob, beamwidth;
  static const int minposterior = -16;

  int mode;
  Accum **accum;

  bool use_prior, use_transition, full_variance;
  int hmm_flag;

  Matrix phonetx, phonetxtotal;

  BaumWelch(AcousticModel::Compiled *m, double bw, bool up=0, bool ut=0, bool fv=0) :
    model(m), K(model->state[0].emission.mean.M), D(model->state[0].emission.mean.N), beamwidth(bw), mode(0),
    accum(new Accum*[model->states]), use_prior(up), use_transition(ut), full_variance(fv), hmm_flag(0),
    phonetx(LFL_PHONES, LFL_PHONES), phonetxtotal(LFL_PHONES, 1)
  {
    if (use_prior)      hmm_flag |= AcousticHMM::Flag::UsePrior;
    if (use_transition) hmm_flag |= AcousticHMM::Flag::UseTransit;

    for (int i=0; i<model->states; i++) {
      AcousticModel::State *s = &model->state[i];
      if (s->val.emission_index != i) FATAL("BaumWelch emission_index mismatch ", s->val.emission_index, " != ", i);
      accum[i] = new Accum(K, D);
    }
    Reset();
  }
  ~BaumWelch() { for(int i=0; i<model->states; i++) delete accum[i]; delete [] accum; }

  void Reset() { 
    for (int i=0; i<model->states; i++) {
      Accum *a = accum[i];
      MatrixIter(&a->mixture.mean) a->mixture.mean.row(i)[j] = 0;
      MatrixIter(&a->mixture.diagcov) a->mixture.diagcov.row(i)[j] = 0;
      MatrixIter(&a->mixture.prior) a->mixture.prior.row(i)[j] = -INFINITY;
      a->total = -INFINITY;
      a->prior = -INFINITY;
      a->transit.clear();
      a->transtotal = -INFINITY;
      a->count = 0;
    }
    totalprob=accumprob=0;
    MatrixIter(&phonetx) phonetx.row(i)[j] = -INFINITY;
    MatrixIter(&phonetxtotal) phonetxtotal.row(i)[j] = -INFINITY;
  }

  void Run(const char *modeldir, const char *featdir, int iterO, int iterI) {
    INFO("enter BaumWelch iteration ", iterO, "-", iterI);
    int count = FeatCorpus::FeatIter(featdir, bind(&BaumWelch::AddFeatures, this, _1, _2, _3, _4));
    INFOf("exit BaumWelch iteration %d-%d, utterances = %d, totalprob = %f, accumprob = %f", iterO, iterI, count, totalprob, accumprob);
  }

  void AddFeatures(const char *fn, Matrix *MFCC, Matrix *features, const char *transcript) {
    if (!DimCheck("BW", features->N, D)) return;

    AcousticModel::Compiled *hmm = AcousticModel::FromUtterance(model, transcript, use_transition);
    if (!hmm) return DEBUG("utterance construct failed: ", transcript);

    double prob = AcousticHMM::ForwardBackward(hmm, features, 2, beamwidth, this);
    if (mode == Mode::Means) totalprob += prob;

    delete hmm;
  }

  void AddPrior(int state, double pi) {
    if (state < 0 || state >= model->states) FATAL("OOB emission index ", state);
    Accum *a = accum[state];
    LogAdd(&a->prior, pi);
  }

  void AddEmission(int state, const double *feature, const double *emissionPosterior, double emissionTotal, double gamma) {
    if (gamma < minposterior) return;
    if (state < 0 || state >= model->states) FATAL("OOB emission index ", state);
    Accum *a = accum[state];

    double *posterior = (double*)alloca(K*sizeof(double));
    for (int i=0; i<K; i++) posterior[i] = emissionPosterior[i] + gamma;
    emissionTotal += gamma;

    if (mode == Mode::Means) {
      double *featscaled = (double*)alloca(D*sizeof(double));
      for (int i=0; i<K; i++) {
        if (emissionPosterior[i] < minposterior) continue;
        Vector::Mult(feature, exp(posterior[i]), featscaled, D);
        Vector::Add(a->mixture.mean.row(i), featscaled, D);
        LogAdd(&a->mixture.prior.row(i)[0], posterior[i]);

        if (!full_variance) {
          Vector::Mult(featscaled, feature, D);
          Vector::Add(a->mixture.diagcov.row(i), featscaled, D);
        }
      }
      a->count++;
      LogAdd(&a->total, emissionTotal); 
      accumprob += emissionTotal;
    }
    else if (mode == Mode::Cov) {
      double *diff = (double*)alloca(D*sizeof(double));
      for (int i=0; i<K; i++) {
        if (emissionPosterior[i] < minposterior) continue;
        Vector::Sub(a->mixture.mean.row(i), feature, diff, D);
        Vector::Mult(diff, diff, D);
        Vector::Mult(diff, exp(posterior[i]), D);
        Vector::Add(a->mixture.diagcov.row(i), diff, D);
      }
      if (!full_variance) FATAL("incompatible modes ", -1);
    }
  }

  void AddTransition(int Lstate, int Rstate, double xi) {
    if (mode != Mode::Means || !isfinite(xi)) return;
    if (Lstate < 0 || Lstate >= model->states) FATAL("OOB emission index ", Lstate);
    if (Rstate < 0 || Rstate >= model->states) FATAL("OOB emission index ", Rstate);
    Accum *a = accum[Lstate];

    Accum::XiMap::iterator i = a->transit.find(Rstate);
    if (i == a->transit.end()) {
      a->transit[Rstate] = -INFINITY;
      i = a->transit.find(Rstate);
    }

    LogAdd(&(*i).second, xi);
    LogAdd(&a->transtotal, xi);

    AcousticModel::State *s1 = &model->state[Lstate], *s2 = &model->state[Rstate];
    int ap, an, phone1 = AcousticModel::ParseName(s1->name, 0, &ap, &an);
    int bp, bn, phone2 = AcousticModel::ParseName(s2->name, 0, &bp, &bn);
    if (phone1 >= 0 && phone2 >= 0 && phone1 != phone2) {
      LogAdd(&phonetx.row(phone1)[phone2], xi);
      LogAdd(&phonetxtotal.row(phone1)[0], xi);
    }
  }

  void Complete() {
    if (mode == Mode::Means) {
      for (int si=0; si<model->states; si++) {
        Accum *a = accum[si];
        for (int i=0; i<K; i++) {
          double sum = a->mixture.prior.row(i)[0];
          double dn = exp(sum);
          if (dn < FLAGS_CovarFloor) dn = FLAGS_CovarFloor;
          Vector::Div(a->mixture.mean.row(i), dn, D);
        }
      }
    } else if (mode == Mode::Cov) {
      double *x = (double*)alloca(D*sizeof(double));
      double totalprior = -INFINITY;
      for (int si=0; si<model->states; si++) {
        Accum *a = accum[si];
        LogAdd(&totalprior, a->prior);
      }

      if (model->phonetx && model->phonetx->M == LFL_PHONES && model->phonetx->N == LFL_PHONES) {
        MatrixIter(model->phonetx) model->phonetx->row(i)[j] = phonetx.row(i)[j] - phonetxtotal.row(i)[0];
      }

      for (int si=0; si<model->states; si++) {
        AcousticModel::State *s = &model->state[si];
        Accum *a = accum[si];
        double lc = log((float)a->count);

        for (int i=0; i<K; i++) {
          double sum = a->mixture.prior.row(i)[0];
          double dn = exp(sum);
          if (dn < FLAGS_CovarFloor) dn = FLAGS_CovarFloor;
          Vector::Div(a->mixture.diagcov.row(i), dn, D);

          Vector::Assign(s->emission.mean.row(i), a->mixture.mean.row(i), D);
          Vector::Assign(s->emission.diagcov.row(i), a->mixture.diagcov.row(i), D);

          if (!full_variance) {
            Vector::Mult(s->emission.mean.row(i), s->emission.mean.row(i), x, D);
            Vector::Sub(s->emission.diagcov.row(i), x, D);
          }

          MatrixColIter(&s->emission.diagcov)
            if (s->emission.diagcov.row(i)[j] < FLAGS_CovarFloor) s->emission.diagcov.row(i)[j] = FLAGS_CovarFloor;

          double prior = sum - lc;
          s->emission.prior.row(i)[0] = prior < FLAGS_PriorFloor ? FLAGS_PriorFloor : prior;

          s->emission.ComputeNorms();

          s->prior = a->prior - totalprior;
        }

        int txind = 0;
        if (s->transition.M != a->transit.size() || s->transition.N != TransitCols) s->transition.Open(a->transit.size(), TransitCols);
        for (Accum::XiMap::iterator i = a->transit.begin(); i != a->transit.end(); i++) {
          AcousticModel::State *s2 = &model->state[(*i).first];
          double *tr = s->transition.row(txind++);
          tr[TC_Self] = fnv32(s->name.c_str());
          tr[TC_Edge] = fnv32(s2->name.c_str());
          tr[TC_Cost] = (*i).second - a->transtotal;
        }
        AcousticModel::State::SortTransitionMap(&s->transition);
      }
      Reset();
    }
  }
};

}; // namespace LFL
#endif // LFL_ML_BAUMWELCH_H__
