# Generated by Django 4.2 on 2024-01-02 01:21

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('storage', '0002_rgbframesequences_current_sequences'),
    ]

    operations = [
        migrations.RenameField(
            model_name='rgbframesequences',
            old_name='current_sequences',
            new_name='current_sequence',
        ),
    ]
