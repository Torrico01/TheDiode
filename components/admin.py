from django.contrib import admin
from .models import Category, ComponentType, Component

# Register your models here.

admin.site.register(Category)
admin.site.register(ComponentType)
admin.site.register(Component)