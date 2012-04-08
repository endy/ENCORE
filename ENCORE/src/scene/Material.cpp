/****************************************************************************
Brandon Light
03/11/2006

Material.cpp

****************************************************************************/

#include "Material.h"

// TODO: remove hardcoded material values ASAP
#define HARDCODED_MATERIAL_VALUES 1

namespace encore
{
	Material::Material()
	{
#if HARDCODED_MATERIAL_VALUES
        m_Ambient = Color(  0.01f, 0.01f, 0.10f, 1.00f );
        m_Diffuse = Color(  0.10f, 1.00f, 0.10f, 1.00f );
        m_Specular = Color( 1.00f, 1.00f, 1.00f, 1.00f );

        m_bEmissive = false;

        m_Reflectivity = 0;

        m_Transmittance = 0.0f;

        m_Transparency = 0;
        m_RefractionIndex = 1;
        m_SpecularExponent = 32;
#else       
        m_Reflectivity = 0.0f;

        m_Transparency = 0.0f;
        m_RefractionIndex = 0.0f;  
#endif
        
	}

	Material::Material(Color diffuse, Color specular, Color ambient)
	{
		m_Diffuse = diffuse;
		m_Specular = specular;
		m_Ambient = ambient;

        m_bEmissive = false;

		m_Reflectivity = 0;

        m_Transmittance = 0.0f;

		m_Transparency = 0;
		m_RefractionIndex = 0;

        m_SpecularExponent = 32;
	}


	Material::~Material()
	{

	}

}