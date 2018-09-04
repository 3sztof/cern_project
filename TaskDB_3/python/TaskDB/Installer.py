# ======================================================================================
# Database maintenance utilities
#
# \author  K.Wilczynski
# \version 1.0
# --------------------------------------------------------------------------------------
import os, logging, API

# ======================================================================================
# Destroy sqllite database instance
#
# \author  K.Wilczynski
# \version 1.0
# --------------------------------------------------------------------------------------
def destroy(database):
    try:
        idx = database.find(':///')
        if idx > 0: db = database[idx+4:]
        else: db = database
        os.stat(db)
        os.unlink(db)
        return True
    except Exception as e:
        logging.error('Exception: '+str(e))
    return False

# ======================================================================================
# Connect to database instance and create the tables
#
# \author  K.Wilczynski
# \version 1.0
# --------------------------------------------------------------------------------------
def install(database):
    try:
        conn = API.Connection(database=database)
        API.Creator(conn).create()
        return True
    except Exception as e:
        logging.error('Exception: '+str(e))

