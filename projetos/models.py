from django.db import models

class PainelArmazenamentoModular(models.Model):
    nome    = models.CharField(max_length=100, unique=True, null=True)
    display = models.FloatField(null=True) # Display de 7 segmentos e 4 dígitos
    #matrizLeds = ...
    modulo  = models.IntegerField(null=True) # Número do módulo em questão (1, 2, 3, ...)
    slot1      = models.ForeignKey('componente.Componente', null=True, unique=True, on_delete=models.CASCADE, related_name='slot1')
    slot2      = models.ForeignKey('componente.Componente', null=True, unique=True, on_delete=models.CASCADE, related_name='slot2')
    slot3      = models.ForeignKey('componente.Componente', null=True, unique=True, on_delete=models.CASCADE, related_name='slot3')
    slot4      = models.ForeignKey('componente.Componente', null=True, unique=True, on_delete=models.CASCADE, related_name='slot4')
    slot5      = models.ForeignKey('componente.Componente', null=True, unique=True, on_delete=models.CASCADE, related_name='slot5')
    slot6      = models.ForeignKey('componente.Componente', null=True, unique=True, on_delete=models.CASCADE, related_name='slot6')
    slot7      = models.ForeignKey('componente.Componente', null=True, unique=True, on_delete=models.CASCADE, related_name='slot7')
    slot8      = models.ForeignKey('componente.Componente', null=True, unique=True, on_delete=models.CASCADE, related_name='slot8')
    slot9      = models.ForeignKey('componente.Componente', null=True, unique=True, on_delete=models.CASCADE, related_name='slot9')

    class Meta:
        verbose_name_plural = "Painéis de armazenamento modular"

    def __str__(self):
        return str(self.nome)