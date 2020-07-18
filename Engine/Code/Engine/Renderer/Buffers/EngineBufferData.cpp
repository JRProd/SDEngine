#include "EngineBufferData.hpp"



bool FrameDataUBO::operator==( const FrameDataUBO& rhs ) const
{
    return !operator!=( rhs );
}

bool FrameDataUBO::operator!=( const FrameDataUBO& rhs ) const
{
    return systemTime != rhs.systemTime || systemDeltaTime != rhs.systemDeltaTime || gameTime != rhs.gameTime ||
        gameDeltaTime != rhs.gameDeltaTime || gamma != rhs.gamma || inverseGamma != rhs.inverseGamma;
}

bool CameraDataUBO::operator==( const CameraDataUBO& rhs ) const
{
    return !operator!=( rhs );
}

bool CameraDataUBO::operator!=( const CameraDataUBO& rhs ) const
{
    return cameraToClipTransform != rhs.cameraToClipTransform || cameraView != rhs.cameraView ||
        position != rhs.position;
}

bool ModelDataUBO::operator==( const ModelDataUBO& rhs ) const
{
    return !operator!=( rhs );
}

bool ModelDataUBO::operator!=( const ModelDataUBO& rhs ) const
{
    return meshModel != rhs.meshModel || meshTint != rhs.meshTint || specularFactor != rhs.specularFactor ||
        specularPower != rhs.specularPower;;
}

ModelDataUBO::ModelDataUBO( const Mat44& model, const Vec4& tint, float factor, float power )
    : meshModel( model )
        , meshTint( tint )
        , specularFactor( factor )
        , specularPower( power )
{
}

bool LightDataUBO::operator==( const LightDataUBO& rhs ) const
{
    return !operator!=( rhs );
}

bool LightDataUBO::operator!=( const LightDataUBO& rhs ) const
{
    if ( ambient != rhs.ambient ) { return true; }

    for ( const Light& light1 : lights )
    {
        for ( const Light& light2 : rhs.lights )
        {
            if ( light1 != light2 ) { return true; }
        }
    }
    return false;
}

bool FogDataUBO::operator==( const FogDataUBO& rhs ) const
{
    return !operator!=( rhs );
}

bool FogDataUBO::operator!=( const FogDataUBO& rhs ) const
{
    return nearFogColor != rhs.nearFogColor || farFogColor != rhs.farFogColor || 
        nearFogDistance != rhs.nearFogDistance || farFogDistance != rhs.farFogDistance;
}
