// These are here to compare with the underlying
// types of the scoped enums
using ui = unsigned int;
using sc = signed char;

enum class ec_ui : ui {};
enum class ec_sc : sc {};
enum e {};

typedef ec_ui td_ec_ui;
typedef ec_sc td_ec_sc;
typedef e td_e;

using u_ec_ui = ec_ui;
using u_ec_sc = ec_sc;
using u_e = e;

extern "C" ui fun_ui(ui x){ return x; }
extern "C" sc fun_sc(sc x){ return x; }
extern "C" ec_ui fun_ec_ui(ec_ui x){ return x; }
extern "C" ec_sc fun_ec_sc(ec_sc x){ return x; }
extern "C" e fun_e(e x){ return x; }
extern "C" td_ec_ui fun_td_ec_ui(td_ec_ui x){ return x; }
extern "C" td_ec_sc fun_td_ec_sc(td_ec_sc x){ return x; }
extern "C" td_e fun_td_e(td_e x){ return x; }
extern "C" u_ec_ui fun_u_ec_ui(u_ec_ui x){ return x; }
extern "C" u_ec_sc fun_u_ec_sc(u_ec_sc x){ return x; }
extern "C" u_e fun_u_e(u_e x){ return x; }
