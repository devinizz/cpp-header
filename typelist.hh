#ifndef TYPELIST_HH_
#define TYPELIST_HH_

namespace TL
{
// declarations
template <class TList> struct Length;
template <class TList, unsigned index> struct TypeAt;
// structure
template <class...> struct TypeList;
template <> struct TypeList<> {};
template <class T, class... Rest> struct TypeList<T, Rest...> {
  typedef T Car;
  typedef TypeList<Rest...> Cdr;
  TypeList() {}
  template <class T_, class... Rest_>
  TypeList(const T_& t, const Rest_&... rest) : car_(t), cdr_(rest...) {}
  const Car& car() const { return car_; }
  Car& car() { return car_; }
  const Cdr& cdr() const { return cdr_; }
  Cdr& cdr() { return cdr_; }
  static unsigned size() { return Length<TypeList>::value; }
  template <unsigned index> const typename TypeAt<TypeList, index>::Result& get() const {
    return TypeAt<TypeList, index>::get(*this);
  }
  template <unsigned index> typename TypeAt<TypeList, index>::Result& get() {
    return TypeAt<TypeList, index>::get(*this);
  }
 private:
  Car car_;
  Cdr cdr_;
};
template <class... ArgsType> inline TypeList<ArgsType...> List(ArgsType... args) {
  return TypeList<ArgsType...>(args...);
}
// cons
template <class T, class TList> struct TypeCons;
template <class T, class... L> struct TypeCons<T, TypeList<L...>> {
  typedef TypeList<T, L...> Result;
};
template <class T, class TList>
inline typename TypeCons<T, TList>::Result Cons(const T& t, const TList& t_list) {
  return typename TypeCons<T, TList>::Result(t, t_list);
}
// get length
template <class TList> struct Length {
  enum { value = 1 + Length<typename TList::Cdr>::value };
};
template <> struct Length<TypeList<> > { enum { value = 0 }; };
// operator[]
template <class TList, unsigned index> struct TypeAt {
  typedef typename TypeAt<typename TList::Cdr, index - 1>::Result Result;
  static const Result& get(const TList& t_list) {
    return TypeAt<typename TList::Cdr, index - 1>::get(t_list.cdr());
  }
  static Result& get(TList& t_list) {
    return TypeAt<typename TList::Cdr, index - 1>::get(t_list.cdr());
  }
};
template <class TList> struct TypeAt<TList, 0> {
  typedef typename TList::Car Result;
  static const Result& get(const TList& t_list) { return t_list.car(); }
  static Result& get(TList& t_list) { return t_list.car(); }
};
// find
template <class TList, class T, unsigned offset = 0> struct IndexOf {
  enum { value = IndexOf<typename TList::Cdr, T, offset + 1>::value };
};
template <class T, unsigned offset> struct IndexOf<TypeList<>, T, offset> {
  enum { value = -1 };
};
template <class T, class... Rest, unsigned offset> struct IndexOf<TypeList<T, Rest...>, T, offset> {
  enum { value = offset };
};
// append
template <class TList, class T> struct Append;
template <class... L, class T> struct Append<TypeList<L...>, T> {
  typedef TypeList<L..., T> Result;
};
template <class... L, class... M> struct Append<TypeList<L...>, TypeList<M...>> {
  typedef TypeList<L..., M...> Result;
};
// erase
template <class TList, class T, bool all = false> struct Erase {
  typedef typename TypeCons<typename TList::Car,
                            typename Erase<typename TList::Cdr, T, all>::Result>::Result Result;
};
template <class T, bool all> struct Erase<TypeList<>, T, all> { typedef TypeList<> Result; };
template <class T, class... Rest> struct Erase<TypeList<T, Rest...>, T, false> {
  typedef TypeList<Rest...> Result;
};
template <class T, class... Rest> struct Erase<TypeList<T, Rest...>, T, true> {
  typedef typename Erase<TypeList<Rest...>, T, true>::Result Result;
};
template <class TList, class T> struct EraseAll {
  typedef typename Erase<TList, T, true>::Result Result;
};
// remove duplicates
template <class TList> struct NoDuplicates;
template <> struct NoDuplicates<TypeList<> > { typedef TypeList<> Result; };
template <class T, class... Rest> struct NoDuplicates<TypeList<T, Rest...> > {
  typedef typename TypeCons<T, typename NoDuplicates<typename EraseAll<TypeList<Rest...>, T>::Result>::Result>::Result Result;
};
// replace
template <class TList, class T, class U, bool all = false> struct Replace {
  typedef typename TypeCons<typename TList::Car,
                            typename Replace<typename TList::Cdr, T, U, all>::Result>::Result Result;
};
template <class T, class U, bool all> struct Replace<TypeList<>, T, U, all> {
  typedef TypeList<> Result;
};
template <class TList, class T, bool all> struct Replace<TList, T, T, all> {
  typedef TList Result;
};
template <class T, class... Rest, class U> struct Replace<TypeList<T, Rest...>, T, U, false> {
  typedef TypeList<U, Rest...> Result;
};
template <class T, class... Rest, class U> struct Replace<TypeList<T, Rest...>, T, U, true> {
  typedef typename TypeCons<U, typename Replace<TypeList<Rest...>, T, U, true>::Result>::Result Result;
};
template <class TList, class T, class U> struct ReplaceAll {
  typedef typename Replace<TList, T, U, true>::Result Result;
};

} // namespace TL

#endif // TYPELIST_HH_
