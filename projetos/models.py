from django.db import models

class PainelDeArmazenamentoModularBase(models.Model):
    nome = models.CharField(max_length=200, null=True, unique=True)
    # Base - Outputs
    display_oled = models.TextField(blank=True, null=True, verbose_name="(Output) Display OLED 0.96'")
    # Base - Inputs
    chave_rotativa = models.CharField(max_length=100, blank=True, null=True, verbose_name="(Input) Chave rotativa")

    class Meta:
        verbose_name_plural = "Bases dos painéis de armazenamento modular"

    def __str__(self):
        return str(self.nome)

class PainelArmazenamentoModular(models.Model):
    nome = models.CharField(max_length=200, null=True, unique=True)
    base = models.ForeignKey('PainelDeArmazenamentoModularBase', on_delete=models.CASCADE, default="1")
    # Painel - Outputs
    display_de_7_segmentos = models.FloatField(blank=True, null=True, verbose_name="(Output) Display de 7 segmentos e 4 dígitos")
    matriz_de_leds = models.JSONField(blank=True, null=True, verbose_name="(Output) Matriz 3x3 de Leds RGB")
    # Painel - Propriedades
    slot_1 = models.OneToOneField('componente.Componente', unique=True, on_delete=models.CASCADE, related_name='slot1', default="1")
    slot_2 = models.OneToOneField('componente.Componente', unique=True, on_delete=models.CASCADE, related_name='slot2', default="1")
    slot_3 = models.OneToOneField('componente.Componente', unique=True, on_delete=models.CASCADE, related_name='slot3', default="1")
    slot_4 = models.OneToOneField('componente.Componente', unique=True, on_delete=models.CASCADE, related_name='slot4', default="1")
    slot_5 = models.OneToOneField('componente.Componente', unique=True, on_delete=models.CASCADE, related_name='slot5', default="1")
    slot_6 = models.OneToOneField('componente.Componente', unique=True, on_delete=models.CASCADE, related_name='slot6', default="1")
    slot_7 = models.OneToOneField('componente.Componente', unique=True, on_delete=models.CASCADE, related_name='slot7', default="1")
    slot_8 = models.OneToOneField('componente.Componente', unique=True, on_delete=models.CASCADE, related_name='slot8', default="1")
    slot_9 = models.OneToOneField('componente.Componente', unique=True, on_delete=models.CASCADE, related_name='slot9', default="1")

    class Meta:
        verbose_name_plural = "Painéis de armazenamento modular"

    def __str__(self):
        return str(self.nome)
    

