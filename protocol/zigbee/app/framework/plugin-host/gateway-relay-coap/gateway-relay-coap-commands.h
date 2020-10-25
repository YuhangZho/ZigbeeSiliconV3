/***************************************************************************//**
 * @file
 * @brief Command definitions for the Gateway Relay COAP plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

typedef struct {
  uint16_t clusterId;
  uint8_t commandId;
  char *format;
  char *signs;
} DotdotCommand;

// Note:  I only need to put the commands here that have arguments.  The
// commands with no arguments need no special rule for handling.
static DotdotCommand dotdotCommandList[] = {
  { ZCL_IDENTIFY_CLUSTER_ID, ZCL_IDENTIFY_COMMAND_ID, "v", NULL },
  { ZCL_IDENTIFY_CLUSTER_ID, ZCL_E_Z_MODE_INVOKE_COMMAND_ID, "u", NULL },
  { ZCL_IDENTIFY_CLUSTER_ID, ZCL_UPDATE_COMMISSION_STATE_COMMAND_ID, "uu", NULL },
  { ZCL_IDENTIFY_CLUSTER_ID, ZCL_IDENTIFY_QUERY_RESPONSE_COMMAND_ID, "v", NULL },
  { ZCL_IDENTIFY_CLUSTER_ID, ZCL_TRIGGER_EFFECT_COMMAND_ID, "uu", NULL },
  { ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_MOVE_TO_LEVEL_COMMAND_ID, "uv", NULL },
  { ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_MOVE_COMMAND_ID, "uu", NULL },
  { ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_STEP_COMMAND_ID, "uuv", NULL },
  { ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_STOP_COMMAND_ID, "uuv", NULL },
  { ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_MOVE_TO_LEVEL_WITH_ON_OFF_COMMAND_ID, "uv", NULL },
  { ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_MOVE_WITH_ON_OFF_COMMAND_ID, "uu", NULL },
  { ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_STEP_WITH_ON_OFF_COMMAND_ID, "uuv", NULL },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_TO_HUE_COMMAND_ID, "uuv", NULL },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_HUE_COMMAND_ID, "uu", NULL },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_STEP_HUE_COMMAND_ID, "uuu", NULL },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_TO_SATURATION_COMMAND_ID, "uv", NULL },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_SATURATION_COMMAND_ID, "uu", NULL },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_STEP_SATURATION_COMMAND_ID, "uuu", NULL },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID, "uuv", NULL },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_TO_COLOR_COMMAND_ID, "vvv", NULL },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_COLOR_COMMAND_ID, "vv", "ss" },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_STEP_COLOR_COMMAND_ID, "vvv", "ssu" },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_TO_COLOR_TEMPERATURE_COMMAND_ID, "vv", NULL },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_COLOR_TEMPERATURE_COMMAND_ID, "uvvv", NULL },
  { ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_STEP_COLOR_TEMPERATURE_COMMAND_ID, "uvvvv", NULL },
  { 0xffff, 0xff, "", NULL }
};
