import sys

if sys.platform == 'win32':
    import asyncio
    asyncio.set_event_loop(asyncio.ProactorEventLoop())

    from .windows_client import Client
else:
    from .linux_client import Client

__all__ = [ 'Client' ]
