# Generated by Django 4.2 on 2024-01-06 15:12

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('storage', '0004_rgbframesequences_speed_rgbframesequences_times'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='rgbframesequences',
            name='speed',
        ),
        migrations.RemoveField(
            model_name='rgbframesequences',
            name='times',
        ),
    ]
