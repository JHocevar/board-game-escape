<script>
  import { onMount } from 'svelte';
  import { base } from '$app/paths';

  let tags = [];
  let error = '';

  onMount(async () => {
    try {
      const response = await fetch('/api/tags');
      if (!response.ok) {
        throw new Error('Unable to load tags');
      }
      tags = await response.json();
    } catch (err) {
      error = err instanceof Error ? err.message : 'Unknown error';
    }
  });
</script>

<svelte:head>
  <title>Tags</title>
</svelte:head>

<main>
  <h1>Tags</h1>
  <p>These are the tags served by the Go backend.</p>
  <nav>
    <a href="{base}/">Home</a>
  </nav>

  {#if error}
    <p class="error">{error}</p>
  {:else if tags.length === 0}
    <p>No tags available yet.</p>
  {:else}
    <ul>
      {#each tags as tag}
        <li><strong>{tag.Id}</strong>: {tag.Content}</li>
      {/each}
    </ul>
  {/if}
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

  .error {
    color: #b91c1c;
  }
</style>
