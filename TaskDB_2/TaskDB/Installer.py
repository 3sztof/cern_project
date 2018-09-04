# ======================================================================================
# Database maintenance utilities
#
# \author  K.Wilczynski
# \version 1.0
# --------------------------------------------------------------------------------------
import os, API

# ======================================================================================
# Destroy sqllite database instance
#
# \author  K.Wilczynski
# \version 1.0
# --------------------------------------------------------------------------------------
def destroy(database):
    try:
        os.stat(database)
        os.unlink(database)
        return True
    except Exception,X:
        print 'Exception: '+str(X)
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
    except Exception,X:
        print 'Exception: '+str(X)
