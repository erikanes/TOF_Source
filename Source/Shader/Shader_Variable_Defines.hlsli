// Matrix
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// Camera
float4 g_vCamPosition;
float3 g_vCamDirection;
float g_fCameraNear;
float g_fCameraFar;

// Light
float4 g_vLightPosition;
float4 g_vLightDirection;
int g_iLightType;

// Generic
bool g_bIsInstanced;
int g_iProjType;

// Outline
float g_fOutlineWidth;