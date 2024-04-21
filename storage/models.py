from django.db import models

class RGBFrameSequences(models.Model):
    name      = models.CharField(max_length=100, unique=True, null=True)
    rgb_strip = models.JSONField(blank=True, null=True, verbose_name="RGB strip sequence pattern (json)")
    current_sequence = models.PositiveIntegerField(default=0)

    def __str__(self):
        return self.name
    
    class Meta:
        verbose_name_plural = "RGB frame sequence patterns"
