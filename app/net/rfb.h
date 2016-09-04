/*
 * $Id: network.h 1335 2014-12-02 04:13:46Z justin $
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

#ifndef LFL_CORE_APP_NET_RFB_H__
#define LFL_CORE_APP_NET_RFB_H__
namespace LFL {
  
struct RFBClient {
  typedef function<bool(string*)> LoadPasswordCB;
  typedef function<void(Connection*, const StringPiece&)> UpdateCB;
  struct Params { string hostport, user; };
  static Connection *Open(Params params, LoadPasswordCB pcb, UpdateCB fcb,
                          Callback *detach=0, Callback *success=0);
};

}; // namespace LFL
#endif // LFL_CORE_APP_NET_RFB_H__
