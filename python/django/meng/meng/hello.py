from django.http import HttpResponse
from datetime import datetime
from django.shortcuts import render

def time(request):
    now = datetime.now()
    browser = request.META.get('HTTP_USER_AGENT', 'unknown')
    return render(request, 'current_time.html', { 'current_time' : now,
                                'user_browser' : browser })


def display_head(request):
    return render(request, 'display_head.html', { 'request' : request })
