/*
 * $Id: camera.cpp 1330 2014-11-06 03:04:15Z justin $
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

namespace LFL {
int Application::GetMaxVolume() { return 0; }
int Application::GetVolume() { return 0; }
void Application::SetVolume(int v) {}
void Application::PlayBackgroundMusic(SoundAsset *music) {}
void Application::PlaySoundEffect(SoundAsset *sa, const v3 &pos, const v3 &vel) {}
unique_ptr<Module> CreateAudioModule(Audio *a) { return nullptr; }
}; // namespace LFL
