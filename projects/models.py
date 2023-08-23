from django.contrib.contenttypes.fields import GenericForeignKey
from django.contrib.contenttypes.models import ContentType
from django.db import models

class ModularStoragePanelBase(models.Model):
    name = models.CharField(max_length=200, null=True, unique=True)
    module = models.PositiveIntegerField(default=0, verbose_name="Module selected")
    # Base - Outputs
    display_oled = models.TextField(blank=True, null=True, verbose_name="(Output) Display OLED 0.96'")
    # Base - Inputs
    rotary_switch = models.CharField(max_length=100, blank=True, null=True, verbose_name="(Input) Chave rotativa")
    # Painel - Interface connections panel
    grid_row = models.PositiveIntegerField(blank=True, null=True)
    grid_col = models.PositiveIntegerField(blank=True, null=True)

    def __str__(self):
        return str(self.name)

class ModularStoragePanel(models.Model):
    name = models.CharField(max_length=200, null=True, unique=True)
    base = models.ForeignKey('ModularStoragePanelBase', on_delete=models.CASCADE, default="1")
    # Painel - Outputs
    display_7_segment = models.FloatField(blank=True, null=True, verbose_name="(Output) Display de 7 segmentos e 4 dígitos")
    leds_matrix = models.JSONField(blank=True, null=True, verbose_name="(Output) Matriz 3x3 de Leds RGB")
    # Painel - Propriedades
    slot_1 = models.OneToOneField('components.Component', unique=True, on_delete=models.CASCADE, related_name='slot1', default="1")
    slot_2 = models.OneToOneField('components.Component', unique=True, on_delete=models.CASCADE, related_name='slot2', default="1")
    slot_3 = models.OneToOneField('components.Component', unique=True, on_delete=models.CASCADE, related_name='slot3', default="1")
    slot_4 = models.OneToOneField('components.Component', unique=True, on_delete=models.CASCADE, related_name='slot4', default="1")
    slot_5 = models.OneToOneField('components.Component', unique=True, on_delete=models.CASCADE, related_name='slot5', default="1")
    slot_6 = models.OneToOneField('components.Component', unique=True, on_delete=models.CASCADE, related_name='slot6', default="1")
    slot_7 = models.OneToOneField('components.Component', unique=True, on_delete=models.CASCADE, related_name='slot7', default="1")
    slot_8 = models.OneToOneField('components.Component', unique=True, on_delete=models.CASCADE, related_name='slot8', default="1")
    slot_9 = models.OneToOneField('components.Component', unique=True, on_delete=models.CASCADE, related_name='slot9', default="1")
    # Painel - Interface connections panel
    grid_row = models.PositiveIntegerField(blank=True, null=True)
    grid_col = models.PositiveIntegerField(blank=True, null=True)

    def __str__(self):
        return str(self.name)
    
class Connection(models.Model):
    name = models.CharField(max_length=200, null=True, unique=True)

    start_type = models.ForeignKey(ContentType, on_delete=models.CASCADE, null=True, related_name="start_type")
    start_object_id = models.PositiveIntegerField(null=True)
    connection_start = GenericForeignKey("start_type", "start_object_id")

    end_type = models.ForeignKey(ContentType, on_delete=models.CASCADE, null=True, related_name="end_type")
    end_object_id = models.PositiveIntegerField(null=True)
    connection_end = GenericForeignKey("end_type", "end_object_id")

    def __str__(self):
        return str(self.name)