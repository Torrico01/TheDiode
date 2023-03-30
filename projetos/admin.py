from django.contrib import admin
from .models import PainelDeArmazenamentoModularBase, PainelArmazenamentoModular

admin.site.register(PainelDeArmazenamentoModularBase)
admin.site.register(PainelArmazenamentoModular)

#@admin.register(PainelArmazenamentoModular)

#class PainelArmazenamentoModularAdmin(admin.ModelAdmin):
#    list_display = ("modulo","display")