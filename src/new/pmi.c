/* Implement subset of PMI functionality on top of pmgr_collective calls */

#include "pmi.h"
#include "spawn_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int initialized  = 0;
static char* server_name = NULL;
spawn_net_channel* server_ch = SPAWN_NET_CHANNEL_NULL;
static int global_ranks;
static int global_rank;
static int global_jobid;

#define MAX_KVS_LEN (256)
#define MAX_KEY_LEN (256)
#define MAX_VAL_LEN (256)

static char kvs_name[MAX_KVS_LEN];

static strmap* put;
static strmap* commit;
static strmap* global;

static const char command_barrier[]  = "BARRIER";
static const char command_finalize[] = "FINALIZE";

int PMI_Init( int *spawned )
{
  /* check that we got a variable to write our flag value to */
  if (spawned == NULL) {
    return PMI_ERR_INVALID_ARG;
  }

  /* we don't support spawned procs */
  *spawned = PMI_FALSE;

  /* allocate new strmaps */
  put = strmap_new();
  commit = strmap_new();
  global = strmap_new();

  const char* value;

  /* read PMI server addr */
  server_name = NULL;
  if ((value = getenv("MV2_PMI_ADDR")) != NULL) {
    server_name = SPAWN_STRDUP(value);
  }
  if (server_name == NULL) {
    return PMI_FAIL;
  }

  /* connect to server */
  server_ch = spawn_net_connect(server_name);
  if (server_ch == SPAWN_NET_CHANNEL_NULL) {
    return PMI_FAIL;
  }

  /* read parameters from server */
  strmap* params = strmap_new();
  spawn_net_read_strmap(server_ch, params);

  /* get rank, ranks, and jobid */
  const char* ranks_str = strmap_get(params, "RANKS");
  global_ranks = atoi(ranks_str);

  const char* rank_str = strmap_get(params, "RANK");
  global_rank = atoi(rank_str);

  const char* jobid_str = strmap_get(params, "JOBID");
  global_jobid = atoi(jobid_str);

  /* create something for our KVS name */
  snprintf(kvs_name, sizeof(kvs_name), "jobid.%d", global_jobid);

  /* delete parameters */
  strmap_delete(&params);

  /* if successful, set initialized=1 */
  initialized = 1;
  return PMI_SUCCESS;
}

int PMI_Initialized( PMI_BOOL *out_initialized )
{
  /* check that we got a variable to write our flag value to */
  if (out_initialized == NULL) {
    return PMI_ERR_INVALID_ARG;
  }

  /* set whether we've initialized or not */
  *out_initialized = PMI_FALSE;
  if (initialized) {
    *out_initialized = PMI_TRUE;
  }
  return PMI_SUCCESS;
}

int PMI_Finalize( void )
{
  int rc = PMI_SUCCESS;

  /* clear put, commit, and global */
  strmap_delete(&global);
  strmap_delete(&commit);
  strmap_delete(&put);

  /* send "FINALIZE" to server */
  spawn_net_write_str(server_ch, command_finalize);

  /* disconnect from parent */
  spawn_net_disconnect(&server_ch);

  return rc;
}

int PMI_Get_size( int *size )
{
  /* check that we're initialized */
  if (!initialized) {
    return PMI_ERR_INIT;
  }

  /* check that we got a variable to write our flag value to */
  if (size == NULL) {
    return PMI_ERR_INVALID_ARG;
  }

  *size = global_ranks;
  return PMI_SUCCESS;
}

int PMI_Get_rank( int *out_rank )
{
  /* check that we're initialized */
  if (!initialized) {
    return PMI_ERR_INIT;
  }

  /* check that we got a variable to write our flag value to */
  if (out_rank == NULL) {
    return PMI_ERR_INVALID_ARG;
  }

  *out_rank = global_rank;
  return PMI_SUCCESS;
}

int PMI_Get_universe_size( int *size )
{
  /* check that we're initialized */
  if (!initialized) {
    return PMI_ERR_INIT;
  }

  /* check that we got a variable to write our flag value to */
  if (size == NULL) {
    return PMI_ERR_INVALID_ARG;
  }

  *size = global_ranks;
  return PMI_SUCCESS;
}

int PMI_Get_appnum( int *appnum )
{
  /* check that we're initialized */
  if (!initialized) {
    return PMI_ERR_INIT;
  }

  /* check that we got a variable to write our flag value to */
  if (appnum == NULL) {
    return PMI_ERR_INVALID_ARG;
  }

  *appnum = global_jobid;
  return PMI_SUCCESS;
}

int PMI_Abort(int exit_code, const char error_msg[])
{
  /* TODO: send "ABORT" message to server */

  /* function prototype requires us to return something */
  return PMI_SUCCESS;
}

int PMI_KVS_Get_my_name( char kvsname[], int length )
{
  /* check that we're initialized */
  if (!initialized) {
    return PMI_ERR_INIT;
  }

  /* check that we got a variable to write our flag value to */
  if (kvsname == NULL) {
    return PMI_ERR_INVALID_ARG;
  }

  /* check that length is large enough */
  if (length < MAX_KVS_LEN) {
    return PMI_ERR_INVALID_LENGTH;
  }

  /* just use the pmgr_id as the kvs space */
  strcpy(kvsname, kvs_name);
  return PMI_SUCCESS;
}

int PMI_KVS_Get_name_length_max( int *length )
{
  /* check that we're initialized */
  if (!initialized) {
    return PMI_ERR_INIT;
  }

  /* check that we got a variable to write our flag value to */
  if (length == NULL) {
    return PMI_ERR_INVALID_ARG;
  }

  *length = MAX_KVS_LEN;
  return PMI_SUCCESS;
}

int PMI_KVS_Get_key_length_max( int *length )
{
  /* check that we're initialized */
  if (!initialized) {
    return PMI_ERR_INIT;
  }

  /* check that we got a variable to write our flag value to */
  if (length == NULL) {
    return PMI_ERR_INVALID_ARG;
  }

  *length = MAX_KEY_LEN;
  return PMI_SUCCESS;
}

int PMI_KVS_Get_value_length_max( int *length )
{
  /* check that we're initialized */
  if (!initialized) {
    return PMI_ERR_INIT;
  }

  /* check that we got a variable to write our flag value to */
  if (length == NULL) {
    return PMI_ERR_INVALID_ARG;
  }

  *length = MAX_VAL_LEN;
  return PMI_SUCCESS;
}

int PMI_KVS_Create( char kvsname[], int length )
{
  /* since we don't support spawning, we just have a static key value space */
  int rc = PMI_KVS_Get_my_name(kvsname, length);
  return rc;
}

int PMI_KVS_Put( const char kvsname[], const char key[], const char value[])
{
  /* check that we're initialized */
  if (!initialized) {
    return PMI_ERR_INIT;
  }

  /* check length of name */
  if (kvsname == NULL || strlen(kvsname) > MAX_KVS_LEN) {
    return PMI_ERR_INVALID_KVS;
  }

  /* check length of key */
  if (key == NULL || strlen(key) > MAX_KEY_LEN) {
    return PMI_ERR_INVALID_KEY;
  }

  /* check length of value */
  if (value == NULL || strlen(value) > MAX_VAL_LEN) {
    return PMI_ERR_INVALID_VAL;
  }

  /* check that kvsname is the correct one */
  if (strcmp(kvsname, kvs_name) != 0) {
    return PMI_ERR_INVALID_KVS;
  }
      
  /* add string to put */
  strmap_set(put, key, value);

  return PMI_SUCCESS;
}

int PMI_KVS_Commit( const char kvsname[] )
{
  /* check that we're initialized */
  if (!initialized) {
    return PMI_ERR_INIT;
  }

  /* check length of name */
  if (kvsname == NULL || strlen(kvsname) > MAX_KVS_LEN) {
    return PMI_ERR_INVALID_KVS;
  }

  /* check that kvsname is the correct one */
  if (strcmp(kvsname, kvs_name) != 0) {
    return PMI_ERR_INVALID_KVS;
  }
      
  /* copy all entries in put to commit,
   * overwrite existing entries */
  strmap_merge(commit, put);

  /* clear put */
  strmap_delete(&put);
  put = strmap_new();

  return PMI_SUCCESS;
}

int PMI_Barrier( void )
{
  /* check that we're initialized */
  if (!initialized) {
    /* would like to return PMI_ERR_INIT here, but definition says
     * it must return either SUCCESS or FAIL, and since user knows
     * that PMI_FAIL == -1, he could be testing for this */
    return PMI_FAIL;
  }

  /* send "BARRIER" message to server */
  spawn_net_write_str(server_ch, command_barrier);

  /* send values in commit */
  spawn_net_write_strmap(server_ch, commit);

  /* clear commit */
  strmap_delete(&commit);
  commit = strmap_new();

  /* recv values in global */
  spawn_net_read_strmap(server_ch, global);

  return PMI_SUCCESS; 
}

int PMI_KVS_Get( const char kvsname[], const char key[], char value[], int length)
{
  /* check that we're initialized */
  if (!initialized) {
    return PMI_ERR_INIT;
  }

  /* check length of name */
  if (kvsname == NULL || strlen(kvsname) > MAX_KVS_LEN) {
    return PMI_ERR_INVALID_KVS;
  }

  /* check that kvsname is the correct one */
  if (strcmp(kvsname, kvs_name) != 0) {
    return PMI_ERR_INVALID_KVS;
  }
      
  /* check length of key */
  if (key == NULL || strlen(key) > MAX_KEY_LEN) {
    return PMI_ERR_INVALID_KEY;
  }

  /* check that we have a buffer to write something to */
  if (value == NULL) {
    return PMI_ERR_INVALID_VAL;
  }

  /* lookup entry from global */
  const char* str = strmap_get(global, key);
  if (str == NULL) {
    /* failed to find the key */
    return PMI_FAIL;
  }

  /* check that the user's buffer is large enough */
  int len = strlen(str) + 1;
  if (length < len) {
    return PMI_ERR_INVALID_LENGTH;
  }

  /* copy the value into user's buffer */
  strcpy(value, str);

  return PMI_SUCCESS;
}

int PMI_Spawn_multiple(
  int count, const char * cmds[], const char ** argvs[], const int maxprocs[],
  const int info_keyval_sizesp[], const PMI_keyval_t * info_keyval_vectors[],
  int preput_keyval_size, const PMI_keyval_t preput_keyval_vector[], int errors[])
{
  /* we don't implement this yet, but mvapich2 needs a reference */
  return PMI_FAIL;
}
