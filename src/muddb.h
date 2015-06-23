extern MYSQL *sql_handle;
extern char *DB_NAME;
extern char *DB_ADDR;
extern char *DB_LOGIN;
extern char *DB_PASSWORD;

bool check_sql( void );
void report_sql_error( MYSQL *con );
bool quick_query( const char *format, ...);
MYSQL_ROW db_query_single_row( const char *query );
bool db_query_list_row( LLIST *list, const char *query );
void debug_row( MYSQL_ROW *row, int size );
void debug_row_list( LLIST *list );
void copy_row( MYSQL_ROW *row_dest, MYSQL_ROW *row_src, int size );
