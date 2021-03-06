// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  Copyright (C) 2013 by Andreas Lauser

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/
/*!
 * \file
 * \copydoc Opm::EclMultiplexerMaterialParams
 */
#ifndef OPM_ECL_MULTIPLEXER_MATERIAL_PARAMS_HPP
#define OPM_ECL_MULTIPLEXER_MATERIAL_PARAMS_HPP

#include "EclStone1Material.hpp"
#include "EclStone2Material.hpp"
#include "EclDefaultMaterial.hpp"
#include "EclTwoPhaseMaterial.hpp"

#include <type_traits>
#include <cassert>
#include <memory>

namespace Opm {

enum EclMultiplexerApproach {
    EclDefaultApproach,
    EclStone1Approach,
    EclStone2Approach,
    EclTwoPhaseApproach
};

/*!
 * \brief Multiplexer implementation for the parameters required by the
 *        multiplexed three-phase material law.
 *
 * Essentially, this class just stores parameter object for the "nested" material law and
 * provides some methods to convert to it.
 */
template<class Traits, class GasOilMaterialLawT, class OilWaterMaterialLawT>
class EclMultiplexerMaterialParams : public Traits
{
    typedef typename Traits::Scalar Scalar;
    enum { numPhases = 3 };

    typedef Opm::EclStone1Material<Traits, GasOilMaterialLawT, OilWaterMaterialLawT> Stone1Material;
    typedef Opm::EclStone2Material<Traits, GasOilMaterialLawT, OilWaterMaterialLawT> Stone2Material;
    typedef Opm::EclDefaultMaterial<Traits, GasOilMaterialLawT, OilWaterMaterialLawT> DefaultMaterial;
    typedef Opm::EclTwoPhaseMaterial<Traits, GasOilMaterialLawT, OilWaterMaterialLawT> TwoPhaseMaterial;

    typedef typename Stone1Material::Params Stone1Params;
    typedef typename Stone2Material::Params Stone2Params;
    typedef typename DefaultMaterial::Params DefaultParams;
    typedef typename TwoPhaseMaterial::Params TwoPhaseParams;

public:
    /*!
     * \brief The multiplexer constructor.
     */
    EclMultiplexerMaterialParams()
    {
        realParams_ = 0;

#ifndef NDEBUG
        finalized_ = false;
#endif
    }

    EclMultiplexerMaterialParams(const EclMultiplexerMaterialParams& /*other*/)
    {
        realParams_ = 0;

#ifndef NDEBUG
        finalized_ = false;
#endif
    }

    ~EclMultiplexerMaterialParams()
    {
        switch (approach()) {
        case EclStone1Approach:
            delete static_cast<Stone1Params*>(realParams_);
            break;

        case EclStone2Approach:
            delete static_cast<Stone2Params*>(realParams_);
            break;

        case EclDefaultApproach:
            delete static_cast<DefaultParams*>(realParams_);
            break;

        case EclTwoPhaseApproach:
            delete static_cast<TwoPhaseParams*>(realParams_);
            break;
        }
    }

    /*!
     * \brief Finish the initialization of the parameter object.
     */
    void finalize()
    {
#ifndef NDEBUG
        finalized_ = true;
#endif
    }

    void setApproach(EclMultiplexerApproach newApproach)
    {
        assert(realParams_ == 0);
        approach_ = newApproach;

        switch (approach()) {
        case EclStone1Approach:
            realParams_ = new Stone1Params;
            break;

        case EclStone2Approach:
            realParams_ = new Stone2Params;
            break;

        case EclDefaultApproach:
            realParams_ = new DefaultParams;
            break;

        case EclTwoPhaseApproach:
            realParams_ = new TwoPhaseParams;
            break;
        }
    }

    EclMultiplexerApproach approach() const
    { return approach_; }

    // get the parameter object for the Stone1 case
    template <EclMultiplexerApproach approachV>
    typename std::enable_if<approachV == EclStone1Approach, Stone1Params>::type&
    getRealParams()
    {
        assert(approach() == approachV);
        return *static_cast<Stone1Params*>(realParams_);
    }

    template <EclMultiplexerApproach approachV>
    typename std::enable_if<approachV == EclStone1Approach, const Stone1Params>::type&
    getRealParams() const
    {
        assert(approach() == approachV);
        return *static_cast<const Stone1Params*>(realParams_);
    }

    // get the parameter object for the Stone2 case
    template <EclMultiplexerApproach approachV>
    typename std::enable_if<approachV == EclStone2Approach, Stone2Params>::type&
    getRealParams()
    {
        assert(approach() == approachV);
        return *static_cast<Stone2Params*>(realParams_);
    }

    template <EclMultiplexerApproach approachV>
    typename std::enable_if<approachV == EclStone2Approach, const Stone2Params>::type&
    getRealParams() const
    {
        assert(approach() == approachV);
        return *static_cast<const Stone2Params*>(realParams_);
    }

    // get the parameter object for the default case
    template <EclMultiplexerApproach approachV>
    typename std::enable_if<approachV == EclDefaultApproach, DefaultParams>::type&
    getRealParams()
    {
        assert(approach() == approachV);
        return *static_cast<DefaultParams*>(realParams_);
    }

    template <EclMultiplexerApproach approachV>
    typename std::enable_if<approachV == EclDefaultApproach, const DefaultParams>::type&
    getRealParams() const
    {
        assert(approach() == approachV);
        return *static_cast<const DefaultParams*>(realParams_);
    }

    // get the parameter object for the twophase case
    template <EclMultiplexerApproach approachV>
    typename std::enable_if<approachV == EclTwoPhaseApproach, TwoPhaseParams>::type&
    getRealParams()
    {
        assert(approach() == approachV);
        return *static_cast<TwoPhaseParams*>(realParams_);
    }

    template <EclMultiplexerApproach approachV>
    typename std::enable_if<approachV == EclTwoPhaseApproach, const TwoPhaseParams>::type&
    getRealParams() const
    {
        assert(approach() == approachV);
        return *static_cast<const TwoPhaseParams*>(realParams_);
    }

private:
#ifndef NDEBUG
    void assertFinalized_() const
    { assert(finalized_); }

    bool finalized_;
#else
    void assertFinalized_() const
    { }
#endif

    EclMultiplexerApproach approach_;
    void* realParams_;
};
} // namespace Opm

#endif
