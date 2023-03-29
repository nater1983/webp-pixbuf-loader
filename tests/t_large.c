#include <gdk-pixbuf/gdk-pixbuf.h>


#define READ_BUFFER_SIZE 65535

gint
main (gint argc, gchar **argv)
{
  GError *error = NULL;
  gchar **env   = g_get_environ ();

  GdkPixbuf *pixbuf
      = gdk_pixbuf_new_from_file (g_environ_getenv (env, "TEST_FILE"), &error);

  if (error)
      g_error ("%s", error->message);

  g_assert_cmpint (gdk_pixbuf_get_width (pixbuf), ==, 4096);
  g_assert_cmpint (gdk_pixbuf_get_height (pixbuf), ==, 4096);

  g_clear_object (&pixbuf);

  GdkPixbufLoader *loader = gdk_pixbuf_loader_new_with_type ("webp", NULL);
  g_assert_nonnull (loader);

  guchar *buffer = g_malloc0 (READ_BUFFER_SIZE);
  GFile *file = g_file_new_for_path (g_environ_getenv (env, "TEST_FILE"));
  g_assert_nonnull (file);
  GInputStream *stream = G_INPUT_STREAM(g_file_read (file, NULL, NULL));
  g_assert_nonnull (stream);

  gsize bytes_read = 0;
  bytes_read = g_input_stream_read (stream, buffer, READ_BUFFER_SIZE, NULL, NULL);
  while (bytes_read > 0) {
    g_message ("%ld", bytes_read);
    gdk_pixbuf_loader_write (loader, buffer, bytes_read, &error);
    if (error)
      g_error ("%s", error->message);

    bytes_read = g_input_stream_read (stream, buffer, READ_BUFFER_SIZE, NULL, &error);
    if (error)
      g_error ("%s", error->message);
  }
  g_assert_cmpint (bytes_read, !=, -1);
  g_input_stream_close (stream, NULL, NULL);

  g_clear_object (&stream);
  g_clear_object (&file);
  g_clear_pointer (&buffer, g_free);
  g_clear_object (&loader);
  g_strfreev (env);
  return 0;
}