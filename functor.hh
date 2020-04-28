#ifndef FUNCTOR_HH_
#define FUNCTOR_HH_
template <class ResultType, class... ArgsType> class FunctorImpl {
 public:
  virtual ResultType operator() (const ArgsType&...) = 0;
  virtual FunctorImpl* Clone() const = 0;
  virtual ~FunctorImpl() {}
};
template <class Fun, class ResultType, class... ArgsType>
class FunctorHandler : public FunctorImpl<ResultType, ArgsType...> {
 private:
  Fun fun_;
 public:
  FunctorHandler(const Fun& fun) : fun_(fun) {}
  FunctorHandler* Clone() const { return new FunctorHandler(*this); }
  ResultType operator() (const ArgsType&... args) { return fun_(args...); }
};
template <typename pObj, typename pFn, class ResultType, class... ArgsType>
class MemFunHandler : public FunctorImpl<ResultType, ArgsType...> {
 private:
  pObj pobj_;
  pFn pfn_;
 public:
  MemFunHandler(const pObj& pobj, const pFn& pfn) : pobj_(pobj), pfn_(pfn) {}
  MemFunHandler* Clone() const { return new MemFunHandler(*this); }
  ResultType operator() (const ArgsType&... args) { return (pobj_->*pfn_)(args...); }
};

template <class> class Functor;
template <class ResultType, class... ArgsType> class Functor<ResultType(ArgsType...)> {
 private:
  typedef FunctorImpl<ResultType, ArgsType...> Impl;
  Impl *pimpl_;
 public:
  Impl* clone_impl() const { return pimpl_ ? pimpl_->Clone() : nullptr; }
  Functor() : pimpl_(nullptr) {}
  Functor(const Functor& fun) : pimpl_(fun.clone_impl()) {}
  template <class Fun> Functor(const Fun& fun) :
      pimpl_(new FunctorHandler<Fun, ResultType, ArgsType...>(fun)) {}
  template <class pObj, class pFn> Functor(const pObj& pobj, const pFn& pfn) :
      pimpl_(new MemFunHandler<pObj, pFn, ResultType, ArgsType...>(pobj, pfn)) {}
  Functor& operator= (const Functor& fun) {
    Impl* tmp = fun.clone_impl();
    delete pimpl_;
    pimpl_ = tmp;
    return *this;
  }
  ResultType operator() (const ArgsType&... args) { return (*pimpl_)(args...); }
  ~Functor() { delete pimpl_; }
};

#endif // FUNCTOR_HH_
