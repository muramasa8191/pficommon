// Copyright (c)2008-2011, Preferred Infrastructure Inc.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Preferred Infrastructure nor the names of other
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <string>
#include <typeinfo>

#include "base.h"

#include "../../data/serialization.h"
#include "../../data/optional.h"

namespace pfi{
namespace text{
namespace json{

template <class T>
T json_cast_impl(const json &v);

template <class T>
T json_cast(const json& v)
{
  try {
    return json_cast_impl<T>(v);
  } catch (std::bad_cast& e) {
    std::string message("Failed json_cast from ");
    message += typeid(*v.get()).name();
    message += " to ";
    message += typeid(T).name();
    message += '.';
    throw json_bad_cast<T>(message);
  }
}

template <class T>
T json_cast_with_default(const json &js, const T &def = T());

template <>
inline int64_t json_cast_impl(const json &js)
{
  const json_integer &p=dynamic_cast<const json_integer&>(*js.get());
  return p.get();
}

template <>
inline int64_t json_cast_with_default(const json &js, const int64_t &def)
{
  const json_integer *p=dynamic_cast<const json_integer*>(js.get());
  return p?p->get():def;
}

template <>
inline int json_cast_impl(const json &js)
{
  return static_cast<int>(json_cast<int64_t>(js));
}

template <>
inline int json_cast_with_default(const json &js, const int &def)
{
  return static_cast<int>(json_cast_with_default<int64_t>(js, def));
}

template <>
inline double json_cast_impl(const json &js)
{
  const json_float &p=dynamic_cast<const json_float&>(*js.get());
  return p.get();
}

template <>
inline double json_cast_with_default(const json &js, const double &def)
{
  const json_float *p=dynamic_cast<const json_float*>(js.get());
  return p?p->get():def;
}

template <>
inline std::string json_cast_impl(const json &js)
{
  const json_string &p=dynamic_cast<const json_string&>(*js.get());
  return p.get();
}

template <>
inline std::string json_cast_with_default(const json &js, const std::string &def)
{
  const json_string *p=dynamic_cast<const json_string*>(js.get());
  return p?p->get():def;
}

template <>
inline bool json_cast_impl(const json &js)
{
  const json_bool &p=dynamic_cast<const json_bool&>(*js.get());
  return p.get();
}

template <>
inline bool json_cast_with_default(const json &js, const bool &def)
{
  const json_bool *p=dynamic_cast<const json_bool*>(js.get());
  return p?p->get():def;
}

template <class T>
inline json to_json(const T &v);

template <class T>
inline void serialize(json &js, T &v)
{
  js=json(new json_object());
  pfi::data::serialization::access::serialize(js, v);
}

template <>
inline void serialize(json &js, json &ks)
{
  js=ks;
}

template <>
inline void serialize(json &js, std::string &s)
{
  js=json(new json_string(s));
}

template <>
inline void serialize(json &js, long long int &n)
{
  js=json(new json_integer(n));
}

template <>
inline void serialize(json &js, unsigned long long int &n)
{
  js=json(new json_integer(n));
}

template <>
inline void serialize(json &js, long int &n)
{
  js=json(new json_integer(n));
}

template <>
inline void serialize(json &js, unsigned long int &n)
{
  js=json(new json_integer(n));
}

template <>
inline void serialize(json &js, int &n)
{
  js=json(new json_integer(n));
}

template <>
inline void serialize(json &js, unsigned int &n)
{
  js=json(new json_integer(n));
}

template <>
inline void serialize(json &js, double &d)
{
  js=json(new json_float(d));
}

template <>
inline void serialize(json &js, bool &b)
{
  js=json(new json_bool(b));
}

template <class T>
inline void serialize(json &js, std::map<std::string, T> &v)
{
  js=json(new json_object());
  for (typename std::map<std::string, T>::iterator it=v.begin();
       it!=v.end(); it++)
    js[it->first]=to_json(it->second);
}

template <class T>
inline void serialize(json &js, std::vector<T> &v)
{
  js=json(new json_array());
  for (size_t i=0; i<v.size(); i++)
    js.add(to_json(v[i]));
}

template <class T>
inline void serialize(json &js, pfi::data::serialization::named_value<T> &v)
{
  js.add(v.name, to_json(v.v));
}

template <class T>
inline void serialize(json &js, pfi::data::optional<T> &v)
{
  if (v)
    js=to_json(*v);
  else
    js=json();
}

template <class T>
inline json to_json(const T &v)
{
  json ret;
  serialize(ret, const_cast<T&>(v));
  return ret;
}

class json_iarchive_cast{
public:
  json_iarchive_cast(const json &js): js(js) {}
  const json &get() const { return js; }

  const static bool is_read = true;
private:
  const json &js;
};

class json_iarchive_cast_with_default : public json_iarchive_cast{
public:
  json_iarchive_cast_with_default(const json &js): json_iarchive_cast(js) {}

  static const bool is_read = true;
};

template <class T>
inline void serialize(json_iarchive_cast &js, T &v)
{
  pfi::data::serialization::access::serialize(js, v);
}

template <class T>
inline void serialize(json_iarchive_cast_with_default &js, T &v)
{
  pfi::data::serialization::access::serialize(js, v);
}

template <>
inline void serialize(json_iarchive_cast &js, json &ks)
{
  ks=js.get();
}

template <>
inline void serialize(json_iarchive_cast_with_default &js, json &ks)
{
  ks=js.get();
}

template <>
inline void serialize(json_iarchive_cast &js, std::string &s)
{
  s=json_cast<std::string>(js.get());
}

template <>
inline void serialize(json_iarchive_cast_with_default &js, std::string &s)
{
  s=json_cast_with_default<std::string>(js.get(), s);
}

template <>
inline void serialize(json_iarchive_cast &js, int &n)
{
  n=json_cast<int>(js.get());
}

template <>
inline void serialize(json_iarchive_cast_with_default &js, int &n)
{
  n=json_cast_with_default<int>(js.get(), n);
}

template <>
inline void serialize(json_iarchive_cast &js, long &n)
{
  n=json_cast<long>(js.get());
}

template <>
inline void serialize(json_iarchive_cast_with_default &js, long &n)
{
  n=json_cast_with_default<long>(js.get(), n);
}

template <>
inline void serialize(json_iarchive_cast &js, long long &n)
{
  n=json_cast<long long>(js.get());
}

template <>
inline void serialize(json_iarchive_cast_with_default &js, long long &n)
{
  n=json_cast_with_default<long long>(js.get(), n);
}

template <>
inline void serialize(json_iarchive_cast &js, double &d)
{
  d=json_cast<double>(js.get());
}

template <>
inline void serialize(json_iarchive_cast_with_default &js, double &d)
{
  d=json_cast_with_default<double>(js.get(), d);
}

template <>
inline void serialize(json_iarchive_cast &js, bool &b)
{
  b=json_cast<bool>(js.get());
}

template <>
inline void serialize(json_iarchive_cast_with_default &js, bool &b)
{
  b=json_cast_with_default<bool>(js.get(), b);
}

template <class T>
inline void serialize(json_iarchive_cast &js, std::map<std::string, T> &v)
{
  v.clear();
  for (json::const_iterator it=js.get().begin();
       it!=js.get().end(); it++)
    from_json(it->second, v[it->first]);
}

template <class T>
inline void serialize(json_iarchive_cast_with_default &js, std::map<std::string, T> &v)
{
  if (!is<json_object>(js.get())) return;

  for (json::const_iterator it=js.get().begin();
       it!=js.get().end(); it++)
    from_json_with_default(it->second, v[it->first]);
}

template <class T>
inline void serialize(json_iarchive_cast &js, std::vector<T> &v)
{
  v.resize(js.get().size());
  for (size_t i=0; i<js.get().size(); i++)
    from_json(js.get()[i], v[i]);
}

template <class T>
inline void serialize(json_iarchive_cast_with_default &js, std::vector<T> &v)
{
  if (!is<json_array>(js.get())) return;

  if (v.size()<js.get().size())
    v.resize(js.get().size());
  for (size_t i=0; i<js.get().size(); i++)
    from_json_with_default(js.get()[i], v[i]);
}

template <class T>
inline void serialize(json_iarchive_cast &js, pfi::data::serialization::named_value<T> &v)
{
  if (js.get().count(v.name))
    from_json(js.get()[v.name], v.v);
  else
    from_json(json(), v.v);
}

template <class T>
inline void serialize(json_iarchive_cast_with_default &js, pfi::data::serialization::named_value<T> &v)
{
  if (!is<json_object>(js.get())) return;
  if (js.get().count(v.name)==0) return;
  from_json_with_default(js.get()[v.name], v.v);
}

template <class T>
inline void serialize(json_iarchive_cast &js, pfi::data::optional<T> &v)
{
  if (is<json_null>(js.get()))
    v=pfi::data::optional<T>();
  else{
    T t;
    serialize(js, t);
    v=t;
  }
}

template <class T>
inline void serialize(json_iarchive_cast_with_default &js, pfi::data::optional<T> &v)
{
  if (is<json_null>(js.get()))
    v=pfi::data::optional<T>();
  else{
    T t;
    serialize(js, t);
    v=t;
  }
}

template <class T>
inline void from_json(const json &js, T &v)
{
  json_iarchive_cast ia(js);
  serialize(ia, v);
}

template <class T>
inline void from_json_with_default(const json &js, T &v)
{
  json_iarchive_cast_with_default ia(js);
  serialize(ia, v);
}

template <class T>
T json_cast_impl(const json &js)
{
  T ret;
  from_json(js, ret);
  return ret;
}

template <class T>
T json_cast_with_default(const json &js, const T &def)
{
  T ret=def;
  from_json_with_default(js, ret);
  return ret;
}

template <class T>
class via_json_wrap{
public:
  via_json_wrap(T &r): r(r) {}
  T &r;
};

template <class T>
via_json_wrap<T> via_json(T &r)
{
  return via_json_wrap<T>(r);
}

template <class T>
inline std::istream &operator>>(std::istream &is, const via_json_wrap<T> &wr)
{
  json j=json_parser(is).parse();
  from_json(j, wr.r);
  return is;
}

template <class T>
class via_json_with_default_wrap{
public:
  via_json_with_default_wrap(T &r): r(r) {}
  T &r;
};

template <class T>
via_json_with_default_wrap<T> via_json_with_default(T &r)
{
  return via_json_with_default_wrap<T>(r);
}

template <class T>
inline std::istream &operator>>(std::istream &is, const via_json_with_default_wrap<T> &wr)
{
  json j=json_parser(is).parse();
  from_json_with_default(j, wr.r);
  return is;
}

} // json
} // text
} // pfi