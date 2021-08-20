from django.contrib import admin
from .models import Componente, Categoria, TipoDeComponente

# Register your models here.

admin.site.register(Categoria)
admin.site.register(TipoDeComponente)
admin.site.register(Componente)