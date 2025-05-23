
#ifndef __CRTP_BASE_HPP__
#define __CRTP_BASE_HPP__

template <typename TImpl>
class CRTPBase {
protected:
    TImpl& Impl() { return static_cast<TImpl&>(*this); }
};

 #endif //__CRTP_BASE_HPP__

