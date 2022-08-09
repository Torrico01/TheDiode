from django.db import models

class PainelArmazenamentoModular(models.Model):
    display = models.FloatField(null=True) # Display de 7 segmentos e 4 dígitos
    #matrizLeds = ...
    modulo  = models.IntegerField(null=True) # Número do módulo em questão (1, 2, 3, ...)

    class Meta:
        verbose_name_plural = "Painéis de armazenamento modular"

    def __str__(self):
        return str(self.modulo)
