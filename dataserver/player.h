struct Player {
	/// 标识符
	uint32 id = 0;
	/// i am a name
	char name[32]  = "";
	float score = 0;
	std::string bin;
	int8 v_int8 = 0;
	uint8 v_uint8 = 0;
	int16 v_int16 = 0;
	uint16 v_uint16 = 0;
	int32 v_int32 = 0;
	uint32 v_uint32 = 0;
	int64 v_int64 = 0;
	uint64 v_uint64 = 0;
	float v_float = 0;
	double v_double = 0;
	bool v_bool = false;
	std::string v_string;
	char v_char[32]  = "";
	std::string v_bytes;
	std::string v_bytes_tiny;
	std::string v_bytes_medium;
	std::string v_bytes_long;
};