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

#ifdef HAVE_CONFIG_H
#include <qlo/config.hpp>
#endif

#include <qlo/calibrationhelpers.hpp>

#include <ql/indexes/iborindex.hpp>
#include <ql/models/shortrate/calibrationhelpers/swaptionhelper.hpp>
#include <ql/models/calibrationhelper.hpp>

#include <oh/repository.hpp>

//using ObjectHandler::ValueObject;
//using ObjectHandler::convert2;
using boost::shared_ptr;
//using boost::dynamic_pointer_cast;
//using std::string;

namespace QuantLibAddin {

    //// Within each of the RateHelper classes we want to remember the ID
    //// of the associated Rate object.  So below we coerce that input
    //// into a string.  If the caller passed in a double instead of a
    //// Rate object then the coerce below will fail in which case we
    //// return an empty string.
    //string f2(const ObjectHandler::property_t &p) {
    //    try {
    //        return convert2<string>(p);
    //    } catch(...) {
    //        return string();
    //    }
    //}

    SwaptionHelper::SwaptionHelper(const shared_ptr<ObjectHandler::ValueObject>& properties,
                                   const QuantLib::Period& maturity,
                                   const QuantLib::Period& length,
                                   const QuantLib::Handle<QuantLib::Quote>& volatility,
                                   const shared_ptr<QuantLib::IborIndex>& index,
                                   const QuantLib::Period& fixedLegTenor,
                                   const QuantLib::DayCounter& fixedLegDayCounter,
                                   const QuantLib::DayCounter& floatingLegDayCounter,
                                   const QuantLib::Handle<QuantLib::YieldTermStructure>& termStructure/*,
                                   CalibrationHelper::CalibrationErrorType errorType
                                   = CalibrationHelper::RelativePriceError,
                                   const Real strike = Null<Real>(),
                                   const Real nominal = 1.0,
                                   const Real shift = 0.0*/,
                                   bool permanent)
    : BlackCalibrationHelper(properties, permanent) {
        libraryObject_ = shared_ptr<QuantLib::BlackCalibrationHelper>(new QuantLib::SwaptionHelper(maturity,
                                                                                              length, 
                                                                                              volatility, 
                                                                                              index, 
                                                                                              fixedLegTenor, 
                                                                                              fixedLegDayCounter, 
                                                                                              floatingLegDayCounter, 
                                                                                              termStructure));
    }


}
