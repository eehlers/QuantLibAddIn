/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005, 2006 Eric Ehlers
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

#ifdef HAVE_CONFIG_H
#include <qlo/config.hpp>
#endif

#include <qlo/shortratemodels.hpp>

#include <ql/models/shortrate/onefactormodels/vasicek.hpp>
#include <ql/models/shortrate/onefactormodels/hullwhite.hpp>
#include <ql/models/shortrate/twofactormodels/g2.hpp>

namespace QuantLibAddin {

    //ShortRateModel::ShortRateModel(
    //            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
    //            //QuantLib::Size nArguments,
    //            bool permanent)
    //: CalibratedModel(properties, permanent){}

    //OneFactorModel::OneFactorModel(
    //            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
    //            //QuantLib::Size nArguments,
    //            bool permanent)
    //: ShortRateModel(properties, permanent){}

    //OneFactorAffineModel::OneFactorAffineModel(
    //           const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
    //           //QuantLib::Size nArguments,
    //           bool permanent) 
    //: OneFactorModel(properties, permanent){}

    OneFactorAffineModel::OneFactorAffineModel(
                const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                //QuantLib::Size nArguments,
                bool permanent) 
    : AffineModel(properties, permanent)/*, OneFactorModel(properties, permanent)*/ {}

    Vasicek::Vasicek(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                     QuantLib::Rate r0,
                     QuantLib::Real a,
                     QuantLib::Real b,
                     QuantLib::Real sigma,
                     QuantLib::Real lambda,
                     bool permanent) : OneFactorAffineModel(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::OneFactorAffineModel>(new
            QuantLib::Vasicek(r0, a, b, sigma, lambda));
    }

    HullWhite::HullWhite(
                const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                const QuantLib::Handle<QuantLib::YieldTermStructure>& hYTS,
                QuantLib::Real a,
                QuantLib::Real sigma,
                bool permanent) : Vasicek(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::OneFactorAffineModel>(new
            QuantLib::HullWhite(hYTS, a, sigma));
    }

    G2::G2(
        const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
        const QuantLib::Handle<QuantLib::YieldTermStructure>& termStructure,
        QuantLib::Real a,
        QuantLib::Real sigma,
        QuantLib::Real b,
        QuantLib::Real eta,
        QuantLib::Real rho,
        bool permanent) : AffineModel(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::AffineModel>(new
            QuantLib::G2(termStructure, a, sigma, b, eta, rho));
    }

}

