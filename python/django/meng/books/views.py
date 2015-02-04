from django.shortcuts import render
from django.http import HttpResponse
import models


def search(request):
    name = request.GET.get('q', None)
    errors = []
    if name is not None:
        if not name:
            errors.append("please enter a term.")
        elif len(name) > 20:
            errors.append("Please enter at most 20 characters.")
        else:
            booklist = models.Book.objects.filter(title__icontains = name)
            return render(request, 'search_results.html', { 'books' : booklist })
    return render(request, 'search_form.html', { 'errors' : errors })
