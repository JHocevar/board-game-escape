<script>
  import { base } from '$app/paths';
</script>

<svelte:head>
  <title>Board Game Escape</title>
</svelte:head>

<main>
  <h1>Board Game Escape</h1>
  <p>A simple landing page for the escape experience.</p>
  <nav>
    <a href="{base}/">Home</a>
    <a href="{base}/tags">View tags</a>
  </nav>
</main>

<style>
  main {
    font-family: sans-serif;
    max-width: 40rem;
    margin: 3rem auto;
    padding: 2rem;
    border: 1px solid #d0d7de;
    border-radius: 12px;
    background: #f8fafc;
  }

  nav {
    display: flex;
    gap: 1rem;
    margin-top: 1rem;
  }

  a {
    color: #2563eb;
    text-decoration: none;
  }
</style>
