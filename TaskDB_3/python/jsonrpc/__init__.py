from .config import Config
config = Config.instance()
from .history import History
history = History.instance()
from .client  import Server, MultiCall, Fault, ProtocolError, loads, dumps
