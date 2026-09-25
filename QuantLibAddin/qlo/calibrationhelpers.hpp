/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2015 Paolo Mazzocchi

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - https://www.quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <https://www.quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#ifndef qla_calibrationhelper_hpp
#define qla_calibrationhelper_hpp

#include <oh/libraryobject.hpp>

#include <ql/types.hpp>


namespace QuantLib {
    class BlackCalibrationHelper;
    class Quote;
    class Period;
    class DayCounter;
    class IborIndex;
    class YieldTermStructure;

    template <class T>
    class Handle;
}

namespace QuantLibAddin {

    class BlackCalibrationHelper : public ObjectHandler::LibraryObject<QuantLib::BlackCalibrationHelper> {
      //public:
      //  std::string quoteName() { return quoteName_; }
      protected:
          OH_LIB_CTOR(BlackCalibrationHelper, QuantLib::BlackCalibrationHelper);
      //  std::string quoteName_;
    };

    class SwaptionHelper : public BlackCalibrationHelper {
      public:
          SwaptionHelper(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                         const QuantLib::Period& maturity,
                         const QuantLib::Period& length,
                         const QuantLib::Handle<QuantLib::Quote>& volatility,
                         const boost::shared_ptr<QuantLib::IborIndex>& index,
                         const QuantLib::Period& fixedLegTenor,
                         const QuantLib::DayCounter& fixedLegDayCounter,
                         const QuantLib::DayCounter& floatingLegDayCounter,
                         const QuantLib::Handle<QuantLib::YieldTermStructure>& termStructure/*,
                         CalibrationHelper::CalibrationErrorType errorType
                         = CalibrationHelper::RelativePriceError,
                         const Real strike = Null<Real>(),
                         const Real nominal = 1.0,
                         const Real shift = 0.0*/,
                         bool permanent);
    };

}

#endif
