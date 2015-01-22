from django.shortcuts import render
from django.http import HttpResponse
import models

def search_form(request):
    return render(request, 'search_form.html')


def search(request):
    name = request.GET.get('q', None)
    if not name:
        return HttpResponse('Please search an item!')

    booklist = models.Book.objects.filter(title__icontains = name)
    return render(request, 'search_results.html', { 'books' : booklist })
