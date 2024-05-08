bool g_bUseDiffuseTexture;	// aiTextureType_DIFFUSE
bool g_bUseNormalTexture;	// aiTextureType_NORMALS
bool g_bUseEmissiveTexture;	// aiTextureType_EMISSIVE
bool g_bUseNoiseTexture;	// aiTextureType_OPACITY
bool g_bUseMaskMapTexture;	// aiTextureType_REFLECTION
bool g_bUseSecondDiffuseTexture;
bool g_bUseSecondNormalTexture;
bool g_bUseLightMapTexture;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_EmissiveTexture;
texture2D g_NoiseTexture;
texture2D g_MaskMapTexture;
texture2D g_SecondDiffuseTexture;
texture2D g_SecondNormalTexture;
texture2D g_LightMapTexture;

vector g_vDiffuseColor = vector(1.f, 1.f, 1.f, 1.f);