import xmlrpclib


def notify(server, title, content, language, target, ids, show):
    '''
    sends a notification through xml-rpc.

    server - a tuple specifying the server, format (ip, port)

    target - an integer specifying whom this notification is sent to
             0 - sent to game, ids is the game list
             1 - sent to CoreClient, ids is ignored
             2 - sent to users, ids is the user list

    show   - where the notification should be displayed
             1 - displayed in CoreClient
             2 - displayed in games
             3 - displayed in both CoreClient and games
    '''
    proxy = xmlrpclib.ServerProxy('http://{}:{}/'.format(*server))
    result = proxy.notif(title, content, language, target, ids, show)
    if result != 'OK':
        logging.info('notify result {}'.format(result))


if __name__ == '__main__':
    notify(('localhost', 9777), 'test', 'this is a test', 'en', 1, ['jack'], 1)
