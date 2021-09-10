#define K int
#define V int
#define P *
#define PP *
typedef map_K_V_t map_K_V_t;
 
V * map_K_V_find_insert_R(map_K_V_t* map_K_V, K P key, V P value);

// V * map_K_V_find_insert_rr(map_K_V_t* map_K_V, K key, V value);
// V * map_K_V_find_insert_R(map_K_V_t* map_K_V, K * key, V value);
// V * map_K_V_find_insert_R(map_K_V_t* map_K_V, K key, V * value);
// V * map_K_V_find_insert_R(map_K_V_t* map_K_V, K * key, V * value);

// for each instance of 'modifiers typename P argname':
//  generate
//      P=*
//      P=
