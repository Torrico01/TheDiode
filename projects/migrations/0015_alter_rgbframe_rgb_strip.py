# Generated by Django 4.2 on 2024-01-06 16:00

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('projects', '0014_rename_current_sequence_rgbframe_current_sequences'),
    ]

    operations = [
        migrations.AlterField(
            model_name='rgbframe',
            name='rgb_strip',
            field=models.JSONField(blank=True, null=True, verbose_name='(Output) Sequence patterns combination (json)'),
        ),
    ]
