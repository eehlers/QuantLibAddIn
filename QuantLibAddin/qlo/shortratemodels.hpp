/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005 Eric Ehlers
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

#ifndef qla_shortratemodels_hpp
#define qla_shortratemodels_hpp

#include <qlo/termstructures.hpp>
#include <qlo/models.hpp>

#include <ql/types.hpp>

namespace QuantLib {
    template <class T>
    class Handle;

    class AffineModel;
    class OneFactorAffineModel;

}

namespace QuantLibAddin {

    OH_LIB_CLASS(AffineModel, QuantLib::AffineModel);

    //class ShortRateModel : public CalibratedModel {
    //public:
    //    ShortRateModel(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
    //                   bool permanent);
    //};

    //class OneFactorModel : public ShortRateModel {
    //public:
    //    OneFactorModel(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
    //                   bool permanent);
    //};

    class OneFactorAffineModel : public AffineModel {
                                 //public OneFactorModel {
     public:
       OneFactorAffineModel(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                            bool permanent);
};

    class Vasicek : public OneFactorAffineModel {
      public:
          Vasicek(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                  QuantLib::Rate r0 = 0.05,
                  QuantLib::Real a = 0.1,
                  QuantLib::Real b = 0.05,
                  QuantLib::Real sigma = 0.01,
                  QuantLib::Real lambda = 0.0,
                  bool permanent = true);
    };

    class HullWhite : public Vasicek {
      public:
        HullWhite(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                  const QuantLib::Handle<QuantLib::YieldTermStructure>& hYTS,
                  QuantLib::Real a,
                  QuantLib::Real sigma,
                  bool permanent);
    };

    class G2 : public AffineModel {
    public:
        G2(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
           const QuantLib::Handle<QuantLib::YieldTermStructure>& termStructure,
           QuantLib::Real a,
           QuantLib::Real sigma,
           QuantLib::Real b,
           QuantLib::Real eta,
           QuantLib::Real rho,
           bool permanent);
    };
}

#endif

