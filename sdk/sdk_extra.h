#pragma once
#include "../includes.h"

#include "settings.h"
#include "hook.h"
#include "sdk/sdk.h"
#include "game_tick.h"
#include "sdk/utils.h"

const FLinearColor white_col  = FLinearColor(1, 1, 1, 1);
const FLinearColor black_col  = FLinearColor(0, 0, 0, 1);
const FLinearColor red_col    = FLinearColor(1, 0, 0, 1);
const FLinearColor yellow_col = FLinearColor(1, 1, 0, 1);
const FLinearColor green_col  = FLinearColor(0, 1, 0, 1);
const FLinearColor cyan_col   = FLinearColor(0, 1, 1, 1);
const FLinearColor blue_col   = FLinearColor(0, 0, 1, 1);
const FLinearColor purple_col = FLinearColor(1, 0, 1, 1);

const FLinearColor sleeping_col = FLinearColor(0.5, 0.86, 0.39, 1);
const FLinearColor dead_col = FLinearColor(0.5, 0.5, 0.5, 1);

float max_arr(float arr[], int n)
{
    int i;
    float max = arr[0];
    for (i = 1; i < n; i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        }
        return max;
    }
}


float min_arr(float arr[], int n)
{
    int i;
    float min = arr[0];
    for (i = 1; i < n; i++)
    {
        if (arr[i] < min)
        {
            min = arr[i];
        }
        return min;
    }
}

typedef struct _D3DXMATRIX
{
    union
    {
        struct
        {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;
        };
        float m[4][4];
    };
} D3DXMATRIX;

D3DXMATRIX matrix(FRotator Rotation, FVector Origin = FVector(0, 0, 0))
{
    float radPitch = (Rotation.Pitch * M_PI / 180.f);
    float radYaw = (Rotation.Yaw * M_PI / 180.f);
    float radRoll = (Rotation.Roll * M_PI / 180.f);
    float SP = sinf(radPitch);
    float CP = cosf(radPitch);
    float SY = sinf(radYaw);
    float CY = cosf(radYaw);
    float SR = sinf(radRoll);
    float CR = cosf(radRoll);
    D3DXMATRIX matrix;
    matrix.m[0][0] = CP * CY;
    matrix.m[0][1] = CP * SY;
    matrix.m[0][2] = SP;
    matrix.m[0][3] = 0.f;
    matrix.m[1][0] = SR * SP * CY - CR * SY;
    matrix.m[1][1] = SR * SP * SY + CR * CY;
    matrix.m[1][2] = -SR * CP;
    matrix.m[1][3] = 0.f;
    matrix.m[2][0] = -(CR * SP * CY + SR * SY);
    matrix.m[2][1] = CY * SR - CR * SP * SY;
    matrix.m[2][2] = CR * CP;
    matrix.m[2][3] = 0.f;
    matrix.m[3][0] = Origin.X;
    matrix.m[3][1] = Origin.Y;
    matrix.m[3][2] = Origin.Z;
    matrix.m[3][3] = 1.f;
    return matrix;
}

bool w2s(FVector world_location, FVector2D& screen_location, APlayerController* player_controller, RECT window_rect)
{
    if (world_location == 0.f) return false;

    FVector location = { 0, 0, 0 };
    FRotator rotation = { 0, 0, 0 };
    location = player_controller->PlayerCameraManager->get_camera_location();
    rotation = player_controller->PlayerCameraManager->get_camera_rotation();

    D3DXMATRIX temp_matrix = matrix(rotation);
    FVector v_axis_x, v_axis_y, v_axis_z;
    v_axis_x = FVector(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
    v_axis_y = FVector(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
    v_axis_z = FVector(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
    float w = temp_matrix.m[3][0] * world_location.X + temp_matrix.m[3][1] * world_location.Y + temp_matrix.m[3][2] * world_location.Z + temp_matrix.m[3][3];
    if (w < 0.01) return false;
    FVector v_delta = world_location - location;
    FVector v_transformed = FVector(v_delta.Dot(v_axis_y), v_delta.Dot(v_axis_z), v_delta.Dot(v_axis_x));
    if (v_transformed.Z < 1.0f) v_transformed.Z = 1.f;
    float fov_angle = player_controller->PlayerCameraManager->DefaultFOV;
    float screen_center_x = (window_rect.right - window_rect.left) / 2;
    float screen_center_y = (window_rect.bottom - window_rect.top) / 2;
    screen_location.X = (screen_center_x + v_transformed.X * (screen_center_x / (float)tan(fov_angle * M_PI / 360)) / v_transformed.Z);
    screen_location.Y = (screen_center_y - v_transformed.Y * (screen_center_x / (float)tan(fov_angle * M_PI / 360)) / v_transformed.Z);
    if (screen_location.X < -50 || screen_location.X >((window_rect.right - window_rect.left) + 250)) return false;
    if (screen_location.Y < -50 || screen_location.Y >(window_rect.bottom - window_rect.top)) return false;
    return true;
}