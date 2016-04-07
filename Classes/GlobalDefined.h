#ifndef _GlobeDefined_H_
#define _GlobeDefined_H_


#define TAG_CTRL_LAYER				1000
#define TAG_CUR_MAIN_LAYER			1001
#define TAG_PROPERTY_LAYER			1002
#define TAG_CONTROL_LAYER			1003
#define TAG_START_LAYER				1004
#define TAG_STAGE_SELECT_LAYER		1005
#define TAG_BOX_PROPERTY_LAYER		1007
#define TAG_PAUSE_LAYER				1008
#define TAG_BACKGOURND_LAYER		1009
#define TAG_BULLET_MENU				1010
#define TAG_LOGO_LAYER				1011
#define TAG_STAGE_OVER_LAYER		1012
#define TAG_TOPBAR					1013
#define TAG_EDIT_MEUN				1014
#define TAG_EDIT_LIST_ADD			1015
#define TAG_EDIT_SAVE_DLG			1016
#define TAG_EDIT_PROPERTY			1017
#define TAG_EDIT_PROPERTY_TYPELIST	1018
#define TAG_EDIT_PROPERTY_RINGLIST	1019

#define TAG_MOVING_ITEM_BASE		10001

#define TAG_ARM_LINE				8999
#define TAG_SELECTED_RECT			9000
#define TAG_LAUNCHER				9001
#define TAG_SLIDER					9002
#define TAG_SLIDER_LABEL			9003
#define TAG_BULLET					9004
#define TAG_EDIT_GRID				9005
#define TAG_RING					9006
#define TAG_BABY					9007
#define TAG_COMBO					9008


#define LONG_PRESS_INTERVAL			60

#define BULLET_SPEED_VALUE			10
#define DEFAULT_BALL_RADIUS			40
#define DEFAULT_BLOCK_EDGE			(2*DEFAULT_BALL_RADIUS)

#define GAME_MODE_PLAY				0
#define GAME_MODE_EDIT				1

#define MSG_ADDENEMYITEM			1
#define MSG_LOADSTAGEFILE			2
#define MSG_CHANGEITEMTYPE			3
#define MSG_CHANGESELECTED			4
#define MSG_DELETESELECTED			5
#define MSG_SAVESTAGE				6
#define MSG_NEWSTAGE				7
#define MSG_LOADSTAGE				8
#define MSG_SETBULLETCOUNT			9
#define MSG_ADDBLOCK				10
#define MSG_DESTROYBULLET			11
#define MSG_CHANGEITEMRINGTYPE		12
#define MSG_CHANGEBOXTYPE			13
#define MSG_LEVELCLEARED			14
#define MSG_SCORE					15
#define MSG_SHOWFLYSCORE			16
#define MSG_SHOWCOMBOCOUNT			17

enum enumZorder
{
	kZorderBackground	= 0,
	kZorderGrid,
	kZoederLauncher,
	kZorderItems,
	kZorderCtrlLayer,
	kZorderPauseLayer,
	kZorderStageOverLayer,
};

enum enumBlockType
{
	kBlockBox,
	kBlockWheel,
	kBlockVortex,
	kBlockBomb,
};

//#define USE_PhysicsIntegration

#define BALLTYPE BallBase::BallType

#define GAME_LOADER_FILE			"game-loader.json"

#ifndef ARRAY_SIZE
#	define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

struct StageRecordItem
{
	StageRecordItem()
	{
		id = 0;
		spendTime = 0;
		score = 0;
	}
	int id;
	int spendTime;
	int score;
};


#endif