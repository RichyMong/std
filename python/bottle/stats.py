from bottle import route, run, template
from arc import monitor

@route('/stats')
def index():

    return template('<b>Hello {{name}}</b>!', name=name)

run(host='localhost', port=8080)

