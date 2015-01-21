from django.db import models

# Create your models here.
class Publisher(models.Model):
    name = models.CharField(max_length = 30)
    address = models.CharField(max_length = 50)
    city = models.CharField(max_length = 60)
    state_province = models.CharField(max_length = 30)
    country = models.CharField(max_length = 50)
    website = models.URLField()

    def __str__(self):
        return '{}'.format(self.name)


class Author(models.Model):
    first_name = models.CharField(max_length = 30)
    last_name = models.CharField(max_length = 40)
    email = models.EmailField(blank = True)

    def __unicode__(self):
        return '{} {}'.format(self.first_name, self.last_name)


class Book(models.Model):
    title = models.CharField(max_length = 100)
    publisher = models.ForeignKey(Publisher)
    publication_date = models.DateField()
    author = models.ManyToManyField(Author)

    def __str__(self):
        return '{}'.format(self.title)