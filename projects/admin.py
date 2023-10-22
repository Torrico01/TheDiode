from django.contrib import admin
from .models import *

admin.site.register(ModularStoragePanelBase)
admin.site.register(ModularStoragePanel3x3)
admin.site.register(Connection)


#@admin.register(PainelArmazenamentoModular)
#class PainelArmazenamentoModularAdmin(admin.ModelAdmin):
#    list_display = ("modulo","display")