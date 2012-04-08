/****************************************************************************
Brandon Light
03/11/2006

Material.h

****************************************************************************/
#pragma once

#include "Color.h"
#include <string>

using std::string;

namespace encore
{
	class Material
	{
	public:
		Material();
		Material(Color diffuse, Color specular, Color ambient);
		virtual ~Material();

    public: // METHODS

		string GetTextureFilename(){ return m_TextureFilename; }
		void   SetTextureFilename(string filename){ m_TextureFilename = filename; }

		Color GetDiffuse(){ return m_Diffuse; }
        const Color GetDiffuse() const {return m_Diffuse; }
		void SetDiffuse(Color diffuse){ m_Diffuse = diffuse; }

		Color GetSpecular(){ return m_Specular; }
        const Color GetSpecular() const { return m_Specular; }
		void SetSpecular(Color specular){ m_Specular = specular; }

		Color GetAmbient(){ return m_Ambient; }
		void SetAmbient(Color ambient){ m_Ambient = ambient; }
		
        void SetEmissive(bool isEmissive) { m_bEmissive = isEmissive; }
        bool IsEmissive(){ return m_bEmissive; }

		float GetReflectivity(){ return m_Reflectivity; }
		void  SetReflectivity(float reflectivity){ m_Reflectivity = reflectivity; }

        float GetAvgDiffuseReflectance(){ return (m_Diffuse.R() + m_Diffuse.G() + m_Diffuse.B()) / 3.0f; }
        
        float GetAvgSpecularReflectance(){ return (m_Specular.R() + m_Specular.G() + m_Specular.B()) / 3.0f; }

        float GetTransmittance(){ return m_Transmittance; }
        void  SetTransimttance(float transmittance){ m_Transmittance = transmittance; }

		float GetTransparency(){ return m_Transparency; }
		void  SetTransparency(float transparency){ m_Transparency = transparency; }
		
		float GetRefractionIndex(){ return m_RefractionIndex; }
		void  SetRefractionIndex(float refractionIndex){ m_RefractionIndex = refractionIndex; }

        float GetSpecularExponent(){ return m_SpecularExponent; }
        void  SetSpecularExponent(float exponent){ m_SpecularExponent = exponent; }
		
	private: // DATA

		Color m_Ambient;
		Color m_Diffuse;
		Color m_Specular;

        bool m_bEmissive;

		float m_Reflectivity;

        float m_Transmittance;

        float m_Transparency;
		float m_RefractionIndex;
        float m_SpecularExponent;

		string m_TextureFilename;
	};
}
