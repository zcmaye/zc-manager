#pragma once

class UserConstants {
public:

	/** 状态：正常 */
	inline static bool ENABLED = 1;

	/** 状态：禁用 */
	inline static bool DISABLED = 0;

	/** 是外链 */
	inline static uint8_t YES_FRAME = 1;

	/** 不是外链 */
	inline static uint8_t NO_FRAME = 0;

	inline static  const char* TYPE_DIR{"D"};
	inline static  const char* TYPE_MENU{"M"};
	inline static  const char* TYPE_BUTTON{"B"};


	inline static  const char* LAYOUT{"Layout"};
	inline static  const char* INNER_LINK{"InnerLink"};
	inline static  const char* PARENT_VIEW{"ParentView"};

	inline static  const char* NO_REDIRECT{"noRedirect"};
};
