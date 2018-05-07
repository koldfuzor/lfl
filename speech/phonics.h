/*
 * $Id$
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

XX(NonBoundary)  
XX(Silence)       YY(SIL)
XX(Stop)          YY(P)  YY(B)  YY(T)  YY(D)  YY(K)  YY(G) 
XX(Nasal)         YY(M)  YY(N)  YY(NG)
XX(Fricative)     YY(S)  YY(SH) YY(Z)  YY(F)  YY(V)  YY(CH) YY(JH) YY(TH) YY(DH)
XX(Liquid)        YY(L)  YY(R)  YY(W)  YY(Y)  YY(HH)
XX(Vowel)         YY(EH) YY(IH) YY(AO) YY(AA) YY(UW) YY(AH) YY(ER) YY(AY) YY(OY) YY(EY) YY(IY) YY(OW)
XX(C-Front)       YY(P)  YY(B)  YY(M)  YY(F)  YY(V)  YY(W)
XX(C-Central)     YY(T)  YY(D)  YY(N)  YY(S)  YY(Z)  YY(SH) YY(TH) YY(DH) YY(L)  YY(R)
XX(C-Back)        YY(SH) YY(CH) YY(JH) YY(Y)  YY(K)  YY(G)  YY(NG) YY(HH)
XX(V-Front)       YY(IY) YY(IH) YY(EH)
XX(V-Central)     YY(EH) YY(AA) YY(ER) YY(AO)
XX(V-Back)        YY(UW) YY(AA) YY(UH)
XX(Front)         YY(P)  YY(B)  YY(M)  YY(F)  YY(V)  YY(W)  YY(IY) YY(IH) YY(EH)
XX(Central)       YY(T)  YY(D)  YY(N)  YY(S)  YY(Z)  YY(SH) YY(TH) YY(DH) YY(L)  YY(R)  YY(EH) YY(AA) YY(ER) YY(AO)
XX(Back)          YY(SH) YY(CH) YY(JH) YY(Y)  YY(K)  YY(G)  YY(NG) YY(HH) YY(AA) YY(UW) YY(UH)
XX(Fortis)        YY(P)  YY(T)  YY(K)  YY(F)  YY(TH) YY(S)  YY(SH) YY(CH)
XX(Lenis)         YY(B)  YY(D)  YY(G)  YY(V)  YY(DH) YY(Z)  YY(SH) YY(JH)
XX(UnFortLenis)   YY(M)  YY(N)  YY(NG) YY(HH) YY(L)  YY(R)  YY(Y)  YY(W)
XX(Coronal)       YY(T)  YY(D)  YY(N)  YY(TH) YY(DH) YY(S)  YY(Z)  YY(SH) YY(CH) YY(JH) YY(L)  YY(R)
XX(NonCoronal)    YY(P)  YY(B)  YY(M)  YY(K)  YY(G)  YY(NG) YY(F)  YY(V)  YY(HH) YY(Y)  YY(W)
XX(Anterior)      YY(P)  YY(B)  YY(M)  YY(T)  YY(D)  YY(N)  YY(F)  YY(V)  YY(TH) YY(DH) YY(S)  YY(Z)  YY(L)  YY(W)
XX(NonAnterior)   YY(K)  YY(G)  YY(NG) YY(SH) YY(HH) YY(CH) YY(JH) YY(R)  YY(Y)
XX(Continuent)    YY(M)  YY(N)  YY(NG) YY(F)  YY(V)  YY(TH) YY(DH) YY(S)  YY(Z)  YY(SH) YY(HH) YY(L)  YY(R)  YY(Y)  YY(W)
XX(NonContinuent) YY(P)  YY(B)  YY(T)  YY(D)  YY(K)  YY(G)  YY(CH) YY(JH)
XX(Strident)      YY(S)  YY(Z)  YY(SH) YY(CH) YY(JH)
XX(NonStrident)   YY(F)  YY(V)  YY(TH) YY(DH) YY(HH)
XX(UnStrident)    YY(P)  YY(B)  YY(M)  YY(T)  YY(D)  YY(N)  YY(K)  YY(G)  YY(NG) YY(L)  YY(R)  YY(Y)  YY(W)
XX(Glide)         YY(HH) YY(L)  YY(R)  YY(Y)  YY(W)
XX(Syllabic)      YY(M)  YY(L)  YY(ER)
XX(Unvoiced-Cons) YY(P)  YY(T)  YY(K)  YY(S)  YY(SH) YY(F)  YY(TH) YY(HH) YY(CH)
XX(Voiced-Cons)   YY(JH) YY(B)  YY(D)  YY(DH) YY(G)  YY(Y)  YY(L)  YY(M)  YY(N)  YY(NG) YY(R)  YY(V)  YY(W)  YY(Z)
XX(Unvoiced-All)  YY(P)  YY(T)  YY(K)  YY(S)  YY(SH) YY(F)  YY(TH) YY(HH) YY(CH) YY(SIL)
XX(Long)          YY(IY) YY(AA) YY(OW) YY(AO) YY(UW) YY(M)  YY(L)  
XX(Short)         YY(EH) YY(EY) YY(AA) YY(IH) YY(AY) YY(OY) YY(AH) YY(UH)
XX(Dipthong)      YY(EY) YY(AY) YY(OY) YY(AA) YY(ER) YY(M)  YY(L)  
XX(Front-Start)   YY(EY) YY(AA) YY(ER)
XX(Fronting)      YY(AY) YY(EY) YY(OY)
XX(High)          YY(IH) YY(UW) YY(AA) YY(IY)
XX(Medium)        YY(EY) YY(ER) YY(AA) YY(EH) YY(M)  YY(L)  
XX(Low)           YY(EH) YY(AY) YY(AA) YY(AW) YY(AO) YY(OY)
XX(Rounded)       YY(AO) YY(UW) YY(AA) YY(OY) YY(W)
XX(Unrounded)     YY(EH) YY(IH) YY(AA) YY(ER) YY(AY) YY(EY) YY(IY) YY(AW) YY(AH)   YY(M) YY(HH) YY(L)  YY(R) YY(Y)
XX(NonAffricate)  YY(S)  YY(SH) YY(Z)  YY(F)  YY(V)  YY(TH) YY(DH)
XX(Affricate)     YY(CH) YY(JH)
XX(IVowel)        YY(IH) YY(IY)
XX(EVowel)        YY(EH) YY(EY)
XX(AVowel)        YY(EH) YY(AA) YY(ER) YY(AY) YY(AW)
XX(OVowel)        YY(AO) YY(OY) YY(AA)
XX(UVowel)        YY(AA) YY(M)  YY(L)  YY(UW)
XX(Voiced-Stop)   YY(B)  YY(D)  YY(G)
XX(Unvoiced-Stop) YY(P)  YY(T)  YY(K) 
XX(Front-Stop)    YY(P)  YY(B)
XX(Central-Stop)  YY(T)  YY(D)  
XX(Back-Stop)     YY(K)  YY(G)
XX(Voiced-Fric)   YY(Z)  YY(SH) YY(DH) YY(CH) YY(V)
XX(Unvoiced-Fric) YY(S)  YY(SH) YY(TH) YY(F)  YY(CH)
XX(Front-Fric)    YY(F)  YY(V)
XX(Central-Fric)  YY(S)  YY(Z)  YY(TH) YY(DH)
XX(Back-Fric)     YY(SH) YY(CH) YY(JH)
XX(aa)            YY(AA)
XX(ae)            YY(AE)
XX(ah)            YY(AH)
XX(ao)            YY(AO)
XX(aw)            YY(AW)
XX(ay)            YY(AY)
XX(b)             YY(B)
XX(ch)            YY(CH)
XX(d)             YY(D)
XX(dh)            YY(DH)
XX(eh)            YY(EH)
XX(er)            YY(ER)
XX(ey)            YY(EY)
XX(f)             YY(F)
XX(g)             YY(G)
XX(hh)            YY(HH)
XX(ih)            YY(IH)
XX(iy)            YY(IY)
XX(jh)            YY(JH)
XX(k)             YY(K)
XX(l)             YY(L)
XX(m)             YY(M)
XX(n)             YY(N)
XX(ng)            YY(NG)
XX(ow)            YY(OW)
XX(oy)            YY(OY)
XX(p)             YY(P)
XX(r)             YY(R)
XX(s)             YY(S)
XX(sh)            YY(SH)
XX(t)             YY(T)
XX(th)            YY(TH)
XX(uh)            YY(UH)
XX(uw)            YY(UW)
XX(v)             YY(V)
XX(w)             YY(W)
XX(y)             YY(Y)
XX(z)             YY(Z)

