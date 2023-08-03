from django.db import models
from pathlib import Path

class Category(models.Model):
    name       = models.CharField(max_length=100, unique=True, null=True)
    quantidade = models.IntegerField(blank=True, null=True)

    def __str__(self):
        return self.name
    
    class Meta:
        verbose_name_plural = "Categories"

class ComponentType(models.Model):
    name       = models.CharField(max_length = 100)
    categoria  = models.ForeignKey('Category', on_delete=models.CASCADE, default="1")
    quantidade = models.IntegerField(blank=True, null=True)

    def __str__(self):
        return self.name

class Component(models.Model):
    name       = models.CharField(max_length=100, null=True)
    tipo       = models.ForeignKey('ComponentType', on_delete=models.CASCADE, default="1")
    quantidade = models.IntegerField(default=0)
    limite     = models.IntegerField(default=99, null=True)

    def __str__(self):
        return self.name
