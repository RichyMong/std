from django.http import HttpResponse
from datetime import datetime

def time(request):
    now = datetime.now()
    return HttpResponse(now)
