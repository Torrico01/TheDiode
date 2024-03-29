# Generated by Django 4.2 on 2023-06-24 20:02

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('projects', '0006_rename_connection_start_connection_content_type'),
    ]

    operations = [
        migrations.RenameField(
            model_name='connection',
            old_name='object_id',
            new_name='start_object_id',
        ),
        migrations.RenameField(
            model_name='connection',
            old_name='content_type',
            new_name='start_type',
        ),
        migrations.AddField(
            model_name='connection',
            name='nome',
            field=models.CharField(max_length=200, null=True, unique=True),
        ),
    ]
