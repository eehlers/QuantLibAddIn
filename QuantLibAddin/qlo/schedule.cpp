/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2011 Ferdinando Ametrano
 Copyright (C) 2006 Eric Ehlers
 Copyright (C) 2005 Aurelien Chanudet

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

#if defined(HAVE_CONFIG_H)     // Dynamically created by configure
    #include <qlo/config.hpp>
#endif
#include <ql/time/schedule.hpp>
#include <qlo/schedule.hpp>

namespace QuantLibAddin {

    Schedule::Schedule(const boost::shared_ptr<ObjectHandler::ValueObject>& p,
                       const QuantLib::Date& effectiveDate,
                       const QuantLib::Date& terminationDate,
                       const QuantLib::Period& tenor,
                       const QuantLib::Calendar& calendar,
                       QuantLib::BusinessDayConvention convention,
                       QuantLib::BusinessDayConvention terminationDateConv,
                       QuantLib::DateGeneration::Rule rule,
                       bool endOfMonth,
                       const QuantLib::Date& firstDate,
                       const QuantLib::Date& nextToLastDate,
                       bool permanent)
    : ObjectHandler::LibraryObject<QuantLib::Schedule>(p, permanent) {
    
        libraryObject_ = boost::shared_ptr<QuantLib::Schedule>(new
            QuantLib::Schedule(effectiveDate,
                               terminationDate,
                               tenor,
                               calendar,
                               convention,
                               terminationDateConv,
                               rule,
                               endOfMonth,
                               firstDate,
                               nextToLastDate));
    }

    Schedule::Schedule(const boost::shared_ptr<ObjectHandler::ValueObject>& p,
                       const std::vector<QuantLib::Date>& dates,
                       bool permanent)
    : ObjectHandler::LibraryObject<QuantLib::Schedule>(p, permanent) {
    
        libraryObject_ = boost::shared_ptr<QuantLib::Schedule>(new
            QuantLib::Schedule(dates));
    }

    Schedule::Schedule(const boost::shared_ptr<ObjectHandler::ValueObject>& p,
        const std::vector<QuantLib::Date>& dates,
        const std::vector<bool>& isRegular,
        const QuantLib::Period& tenor,
        const QuantLib::Calendar& calendar,
        const QuantLib::BusinessDayConvention convention,
        const QuantLib::BusinessDayConvention terminationDateConvention,
        QuantLib::DateGeneration::Rule rule,
        bool endOfMonth,
        bool permanent)
    : ObjectHandler::LibraryObject<QuantLib::Schedule>(p, permanent) {

        QuantLib::ext::optional<QuantLib::BusinessDayConvention> optionalTerminationDateConvention(terminationDateConvention);
        QuantLib::ext::optional<QuantLib::Period> optionalTenor(tenor);
        QuantLib::ext::optional<QuantLib::DateGeneration::Rule> optionalRule(rule);
        QuantLib::ext::optional<bool> optionalEndOfMonth(endOfMonth);

        QuantLib::Schedule schedule(dates,
            calendar,
            convention,
            optionalTerminationDateConvention,
            optionalTenor,
            optionalRule,
            optionalEndOfMonth,
            isRegular);

        libraryObject_ = boost::shared_ptr<QuantLib::Schedule>(new
            QuantLib::Schedule(schedule));
    }

    Schedule::Schedule(const boost::shared_ptr<ObjectHandler::ValueObject>& p,
                       const boost::shared_ptr<QuantLib::Schedule>& from,
                       const QuantLib::Date& truncationDate,
                       const bool returnDatesBefore,
                       bool permanent)
    : ObjectHandler::LibraryObject<QuantLib::Schedule>(p, permanent) {
    
        if (returnDatesBefore)
            libraryObject_ = boost::shared_ptr<QuantLib::Schedule>(new
                QuantLib::Schedule(from->until(truncationDate)));
        else
            libraryObject_ = boost::shared_ptr<QuantLib::Schedule>(new
                QuantLib::Schedule(from->after(truncationDate)));

    }

}
